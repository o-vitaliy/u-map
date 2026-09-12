#include "NoWifiPage.h"

#include "Consts.h"

NoWifiPage::NoWifiPage(Display &display, EasyESPConnect &wifiManager)
    : display_(display), wifiManager_(wifiManager) {}

void NoWifiPage::setup() { showScreen(); }

void NoWifiPage::loop() {}

void NoWifiPage::buttonClick(UiAction action) {
  if (action == UiAction::RIGHT) {
    startPortal();
  } else if (action == UiAction::CANCEL) {
    // Erases saved credentials and restarts the device.
    wifiManager_.resetSettings();
  }
}

void NoWifiPage::showScreen() {
  display_.showMessage("no connection\nClick > to setup\nwifi");
}

// startWebPortal() only sets up the AP + captive portal and returns
// immediately (non-blocking) -- the device restarts itself once the user
// submits credentials through the portal, so there's nothing to wait for
// or report back here.
void NoWifiPage::startPortal() {
  display_.showMessage(String("Connecting...\nOpen ") + Consts::AP_SSID +
                        "\nWiFi to configure");
 Serial.println("startWebPortal");
  wifiManager_.startWebPortal(Consts::AP_SSID);
}
