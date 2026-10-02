#pragma once

#include <Adafruit_NeoPixel.h>

#include "Display.h"
#include "Page.h"

class RegionDebugPage : public Page {
 public:
  RegionDebugPage(Display &display, Adafruit_NeoPixel &leds);

  void setup() override;
  void loop() override;
  void buttonClick(UiAction action) override;

 private:
  Display &display_;
  Adafruit_NeoPixel &leds_;
  size_t selectedRegion_;

  void showSelectedRegion();
};
