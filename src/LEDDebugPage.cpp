#include "LEDDebugPage.h"

namespace {
constexpr uint8_t LED_DATA_PIN = 0;
}

LEDDebugPage::LEDDebugPage(Display &display, Adafruit_NeoPixel &leds)
    : display_(display), leds_(leds),
      selectedColor_(Color::RED) {}

void LEDDebugPage::setup() {
  leds_.begin();
  showSelectedColor();
}

void LEDDebugPage::loop() {}

void LEDDebugPage::buttonClick(UiAction action) {
  if (action != UiAction::LEFT && action != UiAction::RIGHT) {
    return;
  }

  selectedColor_ = static_cast<Color>(
      (static_cast<uint8_t>(selectedColor_) + 1) % 4);
  showSelectedColor();
}

void LEDDebugPage::showSelectedColor() {
  uint32_t color;
  uint8_t red = 0;
  uint8_t green = 0;
  uint8_t blue = 0;
  const char *name;
  switch (selectedColor_) {
    case Color::RED:
      red = 255;
      name = "Red";
      break;
    case Color::GREEN:
      green = 255;
      name = "Green";
      break;
    case Color::YELLOW:
      red = 255;
      green = 255;
      name = "Yellow";
      break;
    case Color::BLUE:
      blue = 255;
      name = "Blue";
      break;
  }

  color = leds_.Color(red, green, blue);
  for (uint16_t index = 0; index < leds_.numPixels(); ++index) {
    leds_.setPixelColor(index, color);
  }
  Serial.printf("LED debug: pin=%u count=%u RGB=(%u,%u,%u)\n",
                LED_DATA_PIN, leds_.numPixels(), red, green, blue);
  leds_.show();
  display_.showMessage(name);
}
