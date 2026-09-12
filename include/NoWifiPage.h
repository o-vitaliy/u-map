#pragma once

#include <EasyESPConnect.h>

#include "Display.h"
#include "Page.h"

class NoWifiPage : public Page {
 public:
  NoWifiPage(Display &display, EasyESPConnect &wifiManager);

  void setup() override;
  void loop() override;
  void buttonClick(UiAction action) override;

 private:
  Display &display_;
  EasyESPConnect &wifiManager_;

  void showScreen();
  void startPortal();
};
