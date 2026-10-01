#pragma once

#include <Adafruit_NeoPixel.h>

#include "Display.h"
#include "Page.h"

class LEDDebugPage : public Page {
 public:
  LEDDebugPage(Display &display, Adafruit_NeoPixel &leds);

  void setup() override;
  void loop() override;
  void buttonClick(UiAction action) override;

 private:
  enum class Color : uint8_t { RED, GREEN, YELLOW, BLUE };

  Display &display_;
  Adafruit_NeoPixel &leds_;
  Color selectedColor_;

  void showSelectedColor();
};
