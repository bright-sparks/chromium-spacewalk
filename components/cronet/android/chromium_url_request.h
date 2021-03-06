// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_CRONET_ANDROID_CHROMIUM_URL_REQUEST_H_
#define COMPONENTS_CRONET_ANDROID_CHROMIUM_URL_REQUEST_H_

#include <jni.h>

namespace cronet {

// Define request priority values like REQUEST_PRIORITY_IDLE in a
// way that ensures they're always the same than their Java counterpart.
enum UrlRequestPriority {
#define DEFINE_REQUEST_PRIORITY(x, y) REQUEST_PRIORITY_##x = y,
#include "components/cronet/android/chromium_url_request_priority_list.h"
#undef DEFINE_REQUEST_PRIORITY
};

// Define request priority values like REQUEST_ERROR_SUCCESS in a
// way that ensures they're always the same than their Java counterpart.
enum UrlRequestError {
#define DEFINE_REQUEST_ERROR(x, y) REQUEST_ERROR_##x = y,
#include "components/cronet/android/chromium_url_request_error_list.h"
#undef DEFINE_REQUEST_ERROR
};

bool ChromiumUrlRequestRegisterJni(JNIEnv* env);

}  // namespace cronet

#endif  // COMPONENTS_CRONET_ANDROID_CHROMIUM_URL_REQUEST_H_
