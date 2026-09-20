#pragma once

#include <atomic>

#include "Component.h"
#include "Display.h"

class ClockComponent : public Component {
 public:
  ClockComponent(Display &display, const char *timezoneInfo);

  void setup() override;
  void loop() override;

 private:
  enum class TimeStatus { SYNCING, SYNCED };

  Display &display_;
  const char *timezoneInfo_;
  unsigned long lastUpdateMs_;
  std::atomic<TimeStatus> timeStatus_;

  void updateDisplay();
  void startTimeSyncTask();
  static void timeSyncTaskEntry(void *param);
};
