// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_COMMON_EXTENSIONS_CHROME_MANIFEST_URL_HANDLERS_H_
#define CHROME_COMMON_EXTENSIONS_CHROME_MANIFEST_URL_HANDLERS_H_

#include <string>

#include "extensions/common/extension.h"
#include "extensions/common/manifest_handler.h"

// Chrome-specific extension manifest URL handlers.

namespace base {
class DictionaryValue;
}

namespace extensions {

namespace chrome_manifest_urls {
const GURL& GetDevToolsPage(const Extension* extension);
}

// Stores Chrome URL overrides specified in extensions manifests.
struct URLOverrides : public Extension::ManifestData {
  typedef std::map<const std::string, GURL> URLOverrideMap;

  URLOverrides();
  virtual ~URLOverrides();

  static const URLOverrideMap& GetChromeURLOverrides(
      const Extension* extension);

  // A map of chrome:// hostnames (newtab, downloads, etc.) to Extension URLs
  // which override the handling of those URLs. (see ExtensionOverrideUI).
  URLOverrideMap chrome_url_overrides_;
};

// Parses the "devtools_page" manifest key.
class DevToolsPageHandler : public ManifestHandler {
 public:
  DevToolsPageHandler();
  virtual ~DevToolsPageHandler();

  virtual bool Parse(Extension* extension, base::string16* error) override;

 private:
  virtual const std::vector<std::string> Keys() const override;

  DISALLOW_COPY_AND_ASSIGN(DevToolsPageHandler);
};

// Parses the "chrome_url_overrides" manifest key.
class URLOverridesHandler : public ManifestHandler {
 public:
  URLOverridesHandler();
  virtual ~URLOverridesHandler();

  virtual bool Parse(Extension* extension, base::string16* error) override;

 private:
  virtual const std::vector<std::string> Keys() const override;

  DISALLOW_COPY_AND_ASSIGN(URLOverridesHandler);
};

}  // namespace extensions

#endif  // CHROME_COMMON_EXTENSIONS_CHROME_MANIFEST_URL_HANDLERS_H_
