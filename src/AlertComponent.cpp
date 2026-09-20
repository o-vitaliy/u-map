#include "AlertComponent.h"

#include "Regions.h"

AlertComponent::AlertComponent(const char *url, unsigned long pollIntervalMs,
                               CRGB *leds, size_t ledCount)
    : alertsClient_(url, pollIntervalMs), leds_(leds), ledCount_(ledCount) {}

void AlertComponent::setup() {
  updateRegionLeds();
}

void AlertComponent::loop() {
  alertsClient_.loop();
  alertsClient_.copyRegionsTo(regions_);
  updateRegionLeds();
}

void AlertComponent::updateRegionLeds() {
  for (size_t i = 0; i < Regions::INDEX_COUNT; ++i) {
    int index = Regions::INDEXES[i].index;
    if (index >= 0 && static_cast<size_t>(index) < ledCount_) {
      leds_[index] = CRGB::White;
    }
  }

  for (const RegionAlert &region : regions_) {
    int index = Regions::indexForName(region.name.c_str());
    if (index < 0 || static_cast<size_t>(index) >= ledCount_) {
      continue;
    }

    leds_[index] = region.alertNow ? CRGB::Red : CRGB::White;
  }
  FastLED.show();
}
