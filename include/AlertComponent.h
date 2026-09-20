#pragma once

#include <FastLED.h>

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
  CRGB *leds_;
  size_t ledCount_;

  void updateRegionLeds();
};
