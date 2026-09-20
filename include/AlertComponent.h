#pragma once

#include <FastLED.h>
#include <vector>

#include "AerialAlertsClient.h"
#include "Component.h"

class AlertComponent : public Component {
 public:
  AlertComponent(const char *url, unsigned long pollIntervalMs, CRGB *leds,
                 size_t ledCount);

  void setup() override;
  void loop() override;

 private:
  AerialAlertsClient alertsClient_;
  std::vector<RegionAlert> regions_;
  CRGB *leds_;
  size_t ledCount_;

  void updateRegionLeds();
};
