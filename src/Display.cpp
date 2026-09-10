#include "Display.h"

#include <Wire.h>

Display::Display(uint8_t sdaPin, uint8_t sclPin, uint8_t width, uint8_t height,
                  uint8_t i2cAddress)
    : sdaPin_(sdaPin),
      sclPin_(sclPin),
      i2cAddress_(i2cAddress),
      panel_(width, height, &Wire, -1) {}

bool Display::begin() {
  Wire.begin(sdaPin_, sclPin_);
  if (!panel_.begin(SSD1306_SWITCHCAPVCC, i2cAddress_)) {
    Serial.println("Display: SSD1306 init failed");
    return false;
  }
  clear();
  return true;
}

void Display::clear() {
  panel_.clearDisplay();
  panel_.display();
}

void Display::showMessage(const String &message) {
  panel_.clearDisplay();
  panel_.setTextSize(1);
  panel_.setTextColor(SSD1306_WHITE);

  const int16_t lineHeight = 8;  // built-in font glyph height at text size 1

  int lineCount = 1;
  for (uint16_t i = 0; i < message.length(); i++) {
    if (message[i] == '\n') {
      lineCount++;
    }
  }

  int16_t y = (panel_.height() - lineCount * lineHeight) / 2;

  int start = 0;
  while (start <= (int)message.length()) {
    int newlineIndex = message.indexOf('\n', start);
    String line = (newlineIndex == -1) ? message.substring(start)
                                        : message.substring(start, newlineIndex);

    int16_t x1, y1;
    uint16_t lineWidth, textHeight;
    panel_.getTextBounds(line, 0, 0, &x1, &y1, &lineWidth, &textHeight);
    panel_.setCursor((panel_.width() - lineWidth) / 2, y);
    panel_.print(line);

    y += lineHeight;
    if (newlineIndex == -1) {
      break;
    }
    start = newlineIndex + 1;
  }

  panel_.display();
}

void Display::showClock(const String &timeText) {
  panel_.clearDisplay();
  drawWifiIcon();

  panel_.setTextSize(3);
  panel_.setTextColor(SSD1306_WHITE);
  int16_t x1, y1;
  uint16_t textWidth, textHeight;
  panel_.getTextBounds(timeText, 0, 0, &x1, &y1, &textWidth, &textHeight);
  panel_.setCursor((panel_.width() - textWidth) / 2,
                    (panel_.height() - textHeight) / 2);
  panel_.print(timeText);
  panel_.display();
}

void Display::drawWifiIcon() {
  const int barWidth = 2;
  const int gap = 1;
  const int barCount = 4;
  const int baseY = 7;
  int x = panel_.width() - barCount * (barWidth + gap);
  for (int i = 0; i < barCount; i++) {
    int barHeight = (i + 1) * 2;
    panel_.fillRect(x + i * (barWidth + gap), baseY - barHeight, barWidth,
                     barHeight, SSD1306_WHITE);
  }
}
