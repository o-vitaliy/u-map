#pragma once

#include <WiFiManager.h>

#include "Display.h"
#include "Page.h"

class NoWifiPage : public Page {
 public:
  NoWifiPage(Display &display, WiFiManager &wifiManager);

  void setup() override;
  void loop() override;
  void buttonClick(UiAction action) override;

 private:
  Display &display_;
  WiFiManager &wifiManager_;

  void showScreen();
  void connectInteractive();
};
