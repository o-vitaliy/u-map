#include "NoWifiPage.h"

NoWifiPage::NoWifiPage(Display &display, WiFiManager &wifiManager)
    : display_(display), wifiManager_(wifiManager) {}

void NoWifiPage::setup() { showScreen(); }

void NoWifiPage::loop() {}

void NoWifiPage::buttonClick(UiAction action) {
  if (action == UiAction::RIGHT) {
    connectInteractive();
  }
}

void NoWifiPage::showScreen() {
  display_.showMessage("no connection\nClick > to setup\nwifi");
}

// Explicitly opens the WiFiManager config portal (blocking) so the user
// can pick a network. Only called in response to a button press, never
// automatically.
void NoWifiPage::connectInteractive() {
  display_.showMessage("Connecting...\nOpen u-lamp-setup\nWiFi to configure");
  wifiManager_.setEnableConfigPortal(true);
  if (wifiManager_.startConfigPortal("u-lamp-setup")) {
    changeState(AppState::CLOCK);
  } else {
    showScreen();
  }
}
