#include "ClockPage.h"

#include <time.h>

namespace {
const unsigned long CLOCK_UPDATE_INTERVAL_MS = 1000;
}  // namespace

ClockPage::ClockPage(Display &display, const char *timezoneInfo)
    : display_(display), timezoneInfo_(timezoneInfo), lastUpdateMs_(0) {}

void ClockPage::setup() {
  configTzTime(timezoneInfo_, "pool.ntp.org", "time.nist.gov");
  lastUpdateMs_ = 0;
  updateClockDisplay();
}

void ClockPage::loop() {
  unsigned long now = millis();
  if (now - lastUpdateMs_ >= CLOCK_UPDATE_INTERVAL_MS) {
    lastUpdateMs_ = now;
    updateClockDisplay();
  }
}

void ClockPage::buttonClick(UiAction action) {
  // Settings menu not implemented yet.
}

void ClockPage::updateClockDisplay() {
  time_t now = time(nullptr);
  if (now < 1700000000) {
    // Not synced via NTP yet.
    display_.showMessage("Time sync...");
    return;
  }

  struct tm timeInfo;
  localtime_r(&now, &timeInfo);
  char buf[6];
  strftime(buf, sizeof(buf), "%H:%M", &timeInfo);
  display_.showClock(buf);
}
