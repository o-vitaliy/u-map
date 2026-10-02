#include "RegionDebugPage.h"

#include "Regions.h"

namespace {
constexpr uint8_t LED_DATA_PIN = 0;
}

RegionDebugPage::RegionDebugPage(Display &display, Adafruit_NeoPixel &leds)
    : display_(display), leds_(leds), selectedRegion_(0) {}

void RegionDebugPage::setup() {
  leds_.begin();
  showSelectedRegion();
}

void RegionDebugPage::loop() {}

void RegionDebugPage::buttonClick(UiAction action) {
  if (action == UiAction::RIGHT) {
    selectedRegion_ = (selectedRegion_ + 1) % Regions::INDEX_COUNT;
  } else if (action == UiAction::LEFT) {
    selectedRegion_ = (selectedRegion_ + Regions::INDEX_COUNT - 1) %
                      Regions::INDEX_COUNT;
  } else {
    return;
  }

  showSelectedRegion();
}

void RegionDebugPage::showSelectedRegion() {
  for (size_t regionIndex = 0; regionIndex < Regions::INDEX_COUNT;
       ++regionIndex) {
    const Regions::RegionIndex &region = Regions::INDEXES[regionIndex];
    if (region.index < 0 || region.index >= leds_.numPixels()) {
      continue;
    }

    uint32_t color = regionIndex == selectedRegion_
                         ? leds_.Color(255, 0, 0)
                         : leds_.Color(255, 255, 255);
    leds_.setPixelColor(region.index, color);
  }

  const Regions::RegionIndex &selected = Regions::INDEXES[selectedRegion_];
  Serial.printf("Region debug: %s, LED %d, count %u\n", selected.name,
                selected.index, leds_.numPixels());
  leds_.show();
  display_.showMessage(selected.name);
}
