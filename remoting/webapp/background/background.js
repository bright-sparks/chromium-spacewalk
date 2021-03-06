// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/** @suppress {duplicate} */
var remoting = remoting || {};

(function(){

/** @return {boolean} */
function isAppsV2() {
  var manifest = chrome.runtime.getManifest();
  if (manifest && manifest.app && manifest.app.background) {
    return true;
  }
  return false;
}

/** @param {remoting.AppLauncher} appLauncher */
function initializeAppV2(appLauncher) {
  /** @type {string} */
  var kNewWindowId = 'new-window';

  /** @param {OnClickData} info */
  function onContextMenu(info) {
    if (info.menuItemId == kNewWindowId) {
      appLauncher.launch();
    }
  }

  function initializeContextMenu() {
    chrome.contextMenus.create({
       id: kNewWindowId,
       contexts: ['launcher'],
       title: chrome.i18n.getMessage(/*i18n-content*/'NEW_WINDOW')
    });
    chrome.contextMenus.onClicked.addListener(onContextMenu);
  }

  initializeContextMenu();
  chrome.app.runtime.onLaunched.addListener(
      appLauncher.launch.bind(appLauncher)
  );
}

/**
 * The background service is responsible for listening to incoming connection
 * requests from Hangouts and the webapp.
 *
 * @param {remoting.AppLauncher} appLauncher
 */
function initializeBackgroundService(appLauncher) {
  function initializeIt2MeService() {
    /** @type {remoting.It2MeService} */
    remoting.it2meService = new remoting.It2MeService(appLauncher);
    remoting.it2meService.init();
  }

  chrome.runtime.onSuspend.addListener(function() {
    base.debug.assert(remoting.it2meService != null);
    remoting.it2meService.dispose();
    remoting.it2meService = null;
  });

  chrome.runtime.onSuspendCanceled.addListener(initializeIt2MeService);
  initializeIt2MeService();
}

function main() {
  /** @type {remoting.AppLauncher} */
  var appLauncher = new remoting.V1AppLauncher();
  if (isAppsV2()) {
    appLauncher = new remoting.V2AppLauncher();
    initializeAppV2(appLauncher);
  }
  initializeBackgroundService(appLauncher);
}

window.addEventListener('load', main, false);

}());
