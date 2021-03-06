// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device/hid/hid_service_linux.h"

#include <linux/hidraw.h>
#include <sys/ioctl.h>
#include <stdint.h>

#include <string>

#include "base/bind.h"
#include "base/files/file.h"
#include "base/files/file_path.h"
#include "base/logging.h"
#include "base/stl_util.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_piece.h"
#include "base/strings/string_split.h"
#include "device/hid/hid_connection_linux.h"
#include "device/hid/hid_device_info.h"
#include "device/hid/hid_report_descriptor.h"
#include "device/udev_linux/udev.h"

#if defined(OS_CHROMEOS)
#include "base/sys_info.h"
#include "chromeos/dbus/dbus_thread_manager.h"
#include "chromeos/dbus/permission_broker_client.h"
#endif  // defined(OS_CHROMEOS)

namespace device {

namespace {

const char kHidrawSubsystem[] = "hidraw";
const char kHIDID[] = "HID_ID";
const char kHIDName[] = "HID_NAME";
const char kHIDUnique[] = "HID_UNIQ";

}  // namespace

HidServiceLinux::HidServiceLinux(
    scoped_refptr<base::MessageLoopProxy> ui_message_loop)
    : ui_message_loop_(ui_message_loop),
      weak_factory_(this) {
  DeviceMonitorLinux* monitor = DeviceMonitorLinux::GetInstance();
  monitor->AddObserver(this);
  monitor->Enumerate(
      base::Bind(&HidServiceLinux::OnDeviceAdded, weak_factory_.GetWeakPtr()));
}

scoped_refptr<HidConnection> HidServiceLinux::Connect(
    const HidDeviceId& device_id) {
  HidDeviceInfo device_info;
  if (!GetDeviceInfo(device_id, &device_info))
    return NULL;

  ScopedUdevDevicePtr device =
      DeviceMonitorLinux::GetInstance()->GetDeviceFromPath(
          device_info.device_id);

  if (device) {
    std::string dev_node = udev_device_get_devnode(device.get());
    return new HidConnectionLinux(device_info, dev_node);
  }

  return NULL;
}

HidServiceLinux::~HidServiceLinux() {
  if (DeviceMonitorLinux::HasInstance())
    DeviceMonitorLinux::GetInstance()->RemoveObserver(this);
}

void HidServiceLinux::OnDeviceAdded(udev_device* device) {
  if (!device)
    return;

  const char* device_path = udev_device_get_syspath(device);
  if (!device_path)
    return;
  const char* subsystem = udev_device_get_subsystem(device);
  if (!subsystem || strcmp(subsystem, kHidrawSubsystem) != 0)
    return;

  scoped_ptr<HidDeviceInfo> device_info(new HidDeviceInfo);
  device_info->device_id = device_path;

  uint32_t int_property = 0;
  const char* str_property = NULL;

  udev_device *parent = udev_device_get_parent(device);
  if (!parent) {
    return;
  }

  const char* hid_id = udev_device_get_property_value(parent, kHIDID);
  if (!hid_id)
    return;

  std::vector<std::string> parts;
  base::SplitString(hid_id, ':', &parts);
  if (parts.size() != 3) {
    return;
  }

  if (HexStringToUInt(base::StringPiece(parts[1]), &int_property)) {
    device_info->vendor_id = int_property;
  }

  if (HexStringToUInt(base::StringPiece(parts[2]), &int_property)) {
    device_info->product_id = int_property;
  }

  str_property = udev_device_get_property_value(parent, kHIDUnique);
  if (str_property != NULL)
    device_info->serial_number = str_property;

  str_property = udev_device_get_property_value(parent, kHIDName);
  if (str_property != NULL)
    device_info->product_name = str_property;

  const std::string dev_node = udev_device_get_devnode(device);
#if defined(OS_CHROMEOS)
  // ChromeOS builds on non-ChromeOS machines (dev) should not attempt to
  // use permission broker.
  if (base::SysInfo::IsRunningOnChromeOS()) {
    chromeos::PermissionBrokerClient* client =
        chromeos::DBusThreadManager::Get()->GetPermissionBrokerClient();
    DCHECK(client) << "Could not get permission broker client.";
    if (!client) {
      return;
    }
    ui_message_loop_->PostTask(
        FROM_HERE,
        base::Bind(&chromeos::PermissionBrokerClient::RequestPathAccess,
                   base::Unretained(client),
                   dev_node,
                   -1,
                   base::Bind(&HidServiceLinux::OnRequestAccessComplete,
                              weak_factory_.GetWeakPtr(),
                              dev_node,
                              base::Passed(&device_info))));
  } else {
    OnRequestAccessComplete(dev_node, device_info.Pass(), true);
  }
#else
  OnRequestAccessComplete(dev_node, device_info.Pass(), true);
#endif  // defined(OS_CHROMEOS)
}

void HidServiceLinux::OnDeviceRemoved(udev_device* device) {
  const char* device_path = udev_device_get_syspath(device);;
  if (device_path)
    RemoveDevice(device_path);
}

void HidServiceLinux::OnRequestAccessComplete(
    const std::string& path,
    scoped_ptr<HidDeviceInfo> device_info,
    bool success) {
  const int flags = base::File::FLAG_OPEN | base::File::FLAG_READ;
  base::File device_file(base::FilePath(path), flags);
  if (!device_file.IsValid()) {
    LOG(ERROR) << "Cannot open '" << path << "': "
        << base::File::ErrorToString(device_file.error_details());
    return;
  }

  int desc_size = 0;
  int res = ioctl(device_file.GetPlatformFile(), HIDIOCGRDESCSIZE, &desc_size);
  if (res < 0) {
    PLOG(ERROR) << "Failed to get report descriptor size";
    device_file.Close();
    return;
  }

  hidraw_report_descriptor rpt_desc;
  rpt_desc.size = desc_size;

  res = ioctl(device_file.GetPlatformFile(), HIDIOCGRDESC, &rpt_desc);
  if (res < 0) {
    PLOG(ERROR) << "Failed to get report descriptor";
    device_file.Close();
    return;
  }

  device_file.Close();

  HidReportDescriptor report_descriptor(rpt_desc.value, rpt_desc.size);
  report_descriptor.GetDetails(&device_info->collections,
                               &device_info->has_report_id,
                               &device_info->max_input_report_size,
                               &device_info->max_output_report_size,
                               &device_info->max_feature_report_size);

  AddDevice(*device_info);
}

}  // namespace device
