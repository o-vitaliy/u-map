#include "MainPage.h"

#include <time.h>

namespace {
const unsigned long CLOCK_UPDATE_INTERVAL_MS = 1000;
const uint32_t CONNECT_TASK_STACK_BYTES = 8192;
const uint32_t TIME_SYNC_TASK_STACK_BYTES = 4096;
const uint32_t TIME_SYNC_RETRY_MS = 5000;
}  // namespace

MainPage::MainPage(Display &display, EasyESPConnect &wifiManager, const char *timezoneInfo)
    : display_(display),
      wifiManager_(wifiManager),
      timezoneInfo_(timezoneInfo),
      lastUpdateMs_(0),
      timeSyncStarted_(false),
      connectStatus_(ConnectStatus::CONNECTING),
      timeStatus_(TimeStatus::SYNCING) {}

void MainPage::setup() {
  connectStatus_ = ConnectStatus::CONNECTING;
  timeStatus_ = TimeStatus::SYNCING;
  timeSyncStarted_ = false;
  display_.showMessage("Connecting...");
  startConnectTask();
}

void MainPage::startConnectTask() {
  BaseType_t created = xTaskCreate(connectTaskEntry, "wifi-connect",
                                    CONNECT_TASK_STACK_BYTES, this, 1, nullptr);
  if (created != pdPASS) {
    Serial.println("MainPage: failed to create wifi connect task");
    connectStatus_ = ConnectStatus::FAILED;
  }
}

// Runs on a dedicated FreeRTOS task so the blocking tryToConnect() call
// doesn't stall the main loop (buttons, display). Only touches WiFi and
// the atomic status flag -- never the Display, which is not safe to
// drive from two tasks at once.
void MainPage::connectTaskEntry(void *param) {
  MainPage *self = static_cast<MainPage *>(param);

  // tryToConnect() never opens the AP/portal itself -- on failure this
  // just falls back to the "no connection" state, and NoWifiPage's RIGHT
  // button explicitly opens the portal from there.
  bool connected = self->wifiManager_.tryToConnect();
  self->connectStatus_ =
      connected ? ConnectStatus::CONNECTED : ConnectStatus::FAILED;

  vTaskDelete(nullptr);
}

void MainPage::startTimeSyncTask() {
  BaseType_t created = xTaskCreate(timeSyncTaskEntry, "time-sync",
                                    TIME_SYNC_TASK_STACK_BYTES, this, 1, nullptr);
  if (created != pdPASS) {
    Serial.println("MainPage: failed to create time sync task");
  }
}

// Runs on a dedicated FreeRTOS task: configures NTP and blocks (via
// getLocalTime()'s own timeout, retried) until the clock is valid. Only
// touches time-related state and the atomic status flag -- never the
// Display.
void MainPage::timeSyncTaskEntry(void *param) {
  MainPage *self = static_cast<MainPage *>(param);

  configTzTime(self->timezoneInfo_, "pool.ntp.org", "time.nist.gov");

  struct tm timeInfo;
  while (!getLocalTime(&timeInfo, TIME_SYNC_RETRY_MS)) {
    // Keep retrying until NTP sync succeeds.
  }
  self->timeStatus_ = TimeStatus::SYNCED;

  vTaskDelete(nullptr);
}

void MainPage::loop() {
  ConnectStatus connectStatus = connectStatus_;

  if (connectStatus == ConnectStatus::CONNECTING) {
    return;
  }

  if (connectStatus == ConnectStatus::FAILED) {
    changeState(AppState::NO_WIFI);
    return;
  }

  if (!timeSyncStarted_) {
    timeSyncStarted_ = true;
    startTimeSyncTask();
  }

  unsigned long now = millis();
  if (now - lastUpdateMs_ < CLOCK_UPDATE_INTERVAL_MS) {
    return;
  }
  lastUpdateMs_ = now;

  if (timeStatus_ == TimeStatus::SYNCING) {
    display_.showMessage("Time sync...");
  } else {
    updateClockDisplay();
  }
}

void MainPage::buttonClick(UiAction action) {
  // Settings menu not implemented yet.
}

void MainPage::updateClockDisplay() {
  time_t now = time(nullptr);
  struct tm timeInfo;
  localtime_r(&now, &timeInfo);
  char buf[6];
  strftime(buf, sizeof(buf), "%H:%M", &timeInfo);
  display_.showClock(buf);
}
