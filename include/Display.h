#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Wraps I2C setup and drawing for an SSD1306 OLED module.
class Display {
 public:
  Display(uint8_t sdaPin, uint8_t sclPin, uint8_t width = 128,
          uint8_t height = 64, uint8_t i2cAddress = 0x3C);

  // Starts I2C on the configured pins and initializes the panel.
  // Returns false if the display isn't found on the bus.
  bool begin();

  void clear();
  void showMessage(const String &message);

  // Draws a large centered time string (e.g. "18:00") with a wifi
  // connected indicator in the top-right corner.
  void showClock(const String &timeText);

 private:
  uint8_t sdaPin_;
  uint8_t sclPin_;
  uint8_t i2cAddress_;
  Adafruit_SSD1306 panel_;

  void drawWifiIcon();
};
