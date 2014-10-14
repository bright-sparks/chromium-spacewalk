// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_COMMON_EXTENSIONS_MANIFEST_URL_HANDLER_H_
#define CHROME_COMMON_EXTENSIONS_MANIFEST_URL_HANDLER_H_

#include <string>

#include "extensions/common/extension.h"
#include "extensions/common/manifest_handler.h"

namespace base {
class DictionaryValue;
}

namespace extensions {

// A structure to hold various URLs like devtools_page, homepage_url, etc
// that may be specified in the manifest of an extension.
struct ManifestURL : public Extension::ManifestData {
  GURL url_;

  // Returns the value of a URL key for an extension, or an empty URL if unset.
  static const GURL& Get(const Extension* extension, const std::string& key);

  // Returns the Homepage URL for this extension.
  // If homepage_url was not specified in the manifest,
  // this returns the Google Gallery URL. For third-party extensions,
  // this returns a blank GURL.
  static const GURL GetHomepageURL(const Extension* extension);

  // Returns true if the extension specified a valid home page url in the
  // manifest.
  static bool SpecifiedHomepageURL(const Extension* extension);

  // Returns the Update URL for this extension.
  static const GURL& GetUpdateURL(const Extension* extension);

  // Returns true if this extension's update URL is the extension gallery.
  static bool UpdatesFromGallery(const Extension* extension);
  static bool UpdatesFromGallery(const base::DictionaryValue* manifest);

  // Returns the About Page for this extension.
  static const GURL& GetAboutPage(const Extension* extension);

  // Returns the webstore page URL for this extension.
  static const GURL GetDetailsURL(const Extension* extension);
};

// Parses the "homepage_url" manifest key.
class HomepageURLHandler : public ManifestHandler {
 public:
  HomepageURLHandler();
  virtual ~HomepageURLHandler();

  virtual bool Parse(Extension* extension, base::string16* error) override;

 private:
  virtual const std::vector<std::string> Keys() const override;

  DISALLOW_COPY_AND_ASSIGN(HomepageURLHandler);
};

// Parses the "update_url" manifest key.
class UpdateURLHandler : public ManifestHandler {
 public:
  UpdateURLHandler();
  virtual ~UpdateURLHandler();

  virtual bool Parse(Extension* extension, base::string16* error) override;

 private:
  virtual const std::vector<std::string> Keys() const override;

  DISALLOW_COPY_AND_ASSIGN(UpdateURLHandler);
};

// Parses the "about_page" manifest key.
// TODO(sashab): Make this and any other similar handlers extend from the same
// abstract class, URLManifestHandler, which has pure virtual methods for
// detecting the required URL type (relative or absolute) and abstracts the
// URL parsing logic away.
class AboutPageHandler : public ManifestHandler {
 public:
  AboutPageHandler();
  virtual ~AboutPageHandler();

  virtual bool Parse(Extension* extension, base::string16* error) override;
  virtual bool Validate(const Extension* extension,
                        std::string* error,
                        std::vector<InstallWarning>* warnings) const override;

 private:
  virtual const std::vector<std::string> Keys() const override;

  DISALLOW_COPY_AND_ASSIGN(AboutPageHandler);
};

}  // namespace extensions

#endif  // CHROME_COMMON_EXTENSIONS_MANIFEST_URL_HANDLER_H_
