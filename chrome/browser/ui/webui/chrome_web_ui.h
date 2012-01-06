// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_WEBUI_CHROME_WEB_UI_H_
#define CHROME_BROWSER_UI_WEBUI_CHROME_WEB_UI_H_
#pragma once

#include "content/browser/webui/web_ui.h"

class ChromeWebUI : public WebUI {
 public:
  explicit ChromeWebUI(content::WebContents* contents);
  virtual ~ChromeWebUI();

  // Overridden from WebUI:
  virtual void RenderViewCreated(RenderViewHost* render_view_host) OVERRIDE;

  // IsMoreWebUI returns a command line flag that tracks whether to use
  // available WebUI implementations of native dialogs.
  static bool IsMoreWebUI();

  // Override the argument setting for more WebUI. If true this enables more
  // WebUI.
  static void OverrideMoreWebUI(bool use_more_webui);

 private:
  DISALLOW_COPY_AND_ASSIGN(ChromeWebUI);
};

#endif  // CHROME_BROWSER_UI_WEBUI_CHROME_WEB_UI_H_
