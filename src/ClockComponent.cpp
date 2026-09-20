#include "ClockComponent.h"

#include <time.h>

namespace {
const unsigned long CLOCK_UPDATE_INTERVAL_MS = 1000;
const uint32_t TIME_SYNC_TASK_STACK_BYTES = 4096;
const uint32_t TIME_SYNC_RETRY_MS = 5000;
}  // namespace

ClockComponent::ClockComponent(Display &display, const char *timezoneInfo)
    : display_(display),
      timezoneInfo_(timezoneInfo),
      lastUpdateMs_(0),
      timeStatus_(TimeStatus::SYNCING) {}

void ClockComponent::setup() {
  timeStatus_ = TimeStatus::SYNCING;
  lastUpdateMs_ = 0;
  display_.showMessage("Time sync...");
  startTimeSyncTask();
}

void ClockComponent::startTimeSyncTask() {
  BaseType_t created = xTaskCreate(timeSyncTaskEntry, "time-sync",
                                  TIME_SYNC_TASK_STACK_BYTES, this, 1, nullptr);
  if (created != pdPASS) {
    Serial.println("ClockComponent: failed to create time sync task");
  }
}

void ClockComponent::timeSyncTaskEntry(void *param) {
  ClockComponent *self = static_cast<ClockComponent *>(param);

  configTzTime(self->timezoneInfo_, "pool.ntp.org", "time.nist.gov");

  struct tm timeInfo;
  while (!getLocalTime(&timeInfo, TIME_SYNC_RETRY_MS)) {
  }
  self->timeStatus_ = TimeStatus::SYNCED;

  vTaskDelete(nullptr);
}

void ClockComponent::loop() {
  unsigned long now = millis();
  if (now - lastUpdateMs_ < CLOCK_UPDATE_INTERVAL_MS) {
    return;
  }
  lastUpdateMs_ = now;

  if (timeStatus_ == TimeStatus::SYNCING) {
    display_.showMessage("Time sync...");
  } else {
    updateDisplay();
  }
}

void ClockComponent::updateDisplay() {
  time_t now = time(nullptr);
  struct tm timeInfo;
  localtime_r(&now, &timeInfo);
  char buffer[6];
  strftime(buffer, sizeof(buffer), "%H:%M", &timeInfo);
  display_.showClock(buffer);
}
