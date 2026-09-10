#pragma once

#include "Display.h"
#include "Page.h"

class ClockPage : public Page {
 public:
  ClockPage(Display &display, const char *timezoneInfo);

  void setup() override;
  void loop() override;
  void buttonClick(UiAction action) override;

 private:
  Display &display_;
  const char *timezoneInfo_;
  unsigned long lastUpdateMs_;

  void updateClockDisplay();
};
