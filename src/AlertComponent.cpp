#include "AlertComponent.h"

#include "Regions.h"

AlertComponent::AlertComponent(const char *url, unsigned long pollIntervalMs,
                               Adafruit_NeoPixel &leds)
    : alertsClient_(url, pollIntervalMs), leds_(leds) {}

void AlertComponent::setup()
{
    leds_.begin();
    for (uint16_t index = 0; index < leds_.numPixels(); ++index)
    {
        leds_.setPixelColor(index, leds_.Color(0, 255, 0));
    }
    leds_.show();
}

void AlertComponent::loop()
{
    alertsClient_.loop();
    alertsClient_.copyRegionsTo(regions_);
    updateRegionLeds();
}

void AlertComponent::updateRegionLeds()
{
    bool changed = false;

    for (size_t i = 0; i < Regions::INDEX_COUNT; ++i)
    {
        const Regions::RegionIndex &region = Regions::INDEXES[i];
        int index = region.index;
        if (index < 0 || index >= leds_.numPixels())
        {
            continue;
        }

        auto alert = regions_.find(region.name);
        bool hasAlert = alert != regions_.end() && alert->second;
        uint32_t desiredColor = hasAlert ? leds_.Color(255, 0, 0)
                                         : leds_.Color(0, 255, 0);
        if (leds_.getPixelColor(index) != desiredColor)
        {
            leds_.setPixelColor(index, desiredColor);
            changed = true;
        }
    }

    if (!changed)
    {
        return;
    }

    for (uint16_t index = 0; index < leds_.numPixels(); ++index)
    {
        Serial.printf("LED %u: (%u, %u, %u)\n", index,
                      (leds_.getPixelColor(index) >> 16) & 0xFF,
                      (leds_.getPixelColor(index) >> 8) & 0xFF,
                      leds_.getPixelColor(index) & 0xFF);
    }

    leds_.show();
}
