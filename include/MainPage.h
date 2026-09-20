#pragma once

#include <EasyESPConnect.h>
#include <atomic>

#include "AlertComponent.h"
#include "ClockComponent.h"
#include "Display.h"
#include "Page.h"

class MainPage : public Page {
 public:
  MainPage(Display &display, EasyESPConnect &wifiManager,
           ClockComponent &clockComponent, AlertComponent &alertComponent);

  void setup() override;
  void loop() override;
  void buttonClick(UiAction action) override;

 private:
  enum class ConnectStatus { CONNECTING, CONNECTED, FAILED };

  Display &display_;
  EasyESPConnect &wifiManager_;
  ClockComponent &clockComponent_;
  AlertComponent &alertComponent_;

  // Written by the background tasks below, read by loop() on the main
  // task. All Display/I2C access and changeState() calls happen from
  // loop(), never from a background task.
  std::atomic<ConnectStatus> connectStatus_;
  bool componentsStarted_;

  void startConnectTask();
  static void connectTaskEntry(void *param);
};
