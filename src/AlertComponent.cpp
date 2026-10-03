#include "AlertComponent.h"

#include "Regions.h"

AlertComponent::AlertComponent(const char *url, unsigned long pollIntervalMs,
                               Adafruit_NeoPixel &leds)
    : alertsClient_(url, pollIntervalMs), leds_(leds) {}

void AlertComponent::setup()
{
    leds_.begin();
    for (size_t i = 0; i < Regions::INDEX_COUNT; ++i)
    {
        const Regions::RegionIndex &region = Regions::INDEXES[i];
        if (region.index < 0 || region.index >= leds_.numPixels())
        {
            continue;
        }

        uint32_t color = region.topHalf ? leds_.Color(0, 0, 255)
                                        : leds_.Color(255, 255, 0);
        leds_.setPixelColor(region.index, color);
    }
    leds_.show();
}

void AlertComponent::loop()
{
    alertsClient_.loop();
    if (!alertsClient_.hasReceivedResponse())
    {
        return;
    }
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

        auto alert = regions_.find(String(region.name));
        uint32_t desiredColor = leds_.Color(128, 128, 128);
        if (alert != regions_.end())
        {
            if (alert->second == AlertLevel::RED)
            {
                desiredColor = leds_.Color(255, 0, 0);
            }
            else if (alert->second == AlertLevel::YELLOW)
            {
                desiredColor = leds_.Color(255, 255, 0);
            }
        }
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
