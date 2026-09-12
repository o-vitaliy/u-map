#pragma once

#include <EasyESPConnect.h>
#include <atomic>

#include "Display.h"
#include "Page.h"

class ClockPage : public Page {
 public:
  ClockPage(Display &display, EasyESPConnect &wifiManager, const char *timezoneInfo);

  void setup() override;
  void loop() override;
  void buttonClick(UiAction action) override;

 private:
  enum class ConnectStatus { CONNECTING, CONNECTED, FAILED };
  enum class TimeStatus { SYNCING, SYNCED };

  Display &display_;
  EasyESPConnect &wifiManager_;
  const char *timezoneInfo_;
  unsigned long lastUpdateMs_;
  bool timeSyncStarted_;

  // Written by the background tasks below, read by loop() on the main
  // task. All Display/I2C access and changeState() calls happen from
  // loop(), never from a background task.
  std::atomic<ConnectStatus> connectStatus_;
  std::atomic<TimeStatus> timeStatus_;

  void updateClockDisplay();

  void startConnectTask();
  static void connectTaskEntry(void *param);

  void startTimeSyncTask();
  static void timeSyncTaskEntry(void *param);
};
