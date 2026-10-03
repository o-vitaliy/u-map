#pragma once

#include <Adafruit_NeoPixel.h>
#include <map>

#include "AerialAlertsClient.h"
#include "Component.h"

class AlertComponent : public Component {
 public:
  AlertComponent(const char *url, unsigned long pollIntervalMs,
                 Adafruit_NeoPixel &leds);

  void setup() override;
  void loop() override;

 private:
  AerialAlertsClient alertsClient_;
  std::map<String, AlertLevel> regions_;
  Adafruit_NeoPixel &leds_;

  void updateRegionLeds();
};
