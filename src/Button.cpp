#include "Button.h"

Button::Button(uint8_t pin, unsigned long longPressMs, unsigned long debounceMs)
    : pin_(pin),
      longPressMs_(longPressMs),
      debounceMs_(debounceMs),
      stableState_(HIGH),
      lastRawState_(HIGH),
      lastEdgeMs_(0),
      pressStartMs_(0),
      longPressFired_(false) {}

void Button::begin() {
  pinMode(pin_, INPUT_PULLUP);
  stableState_ = digitalRead(pin_);
  lastRawState_ = stableState_;
}

void Button::loop() {
  unsigned long now = millis();

  int raw = digitalRead(pin_);
  if (raw != lastRawState_) {
    lastRawState_ = raw;
    lastEdgeMs_ = now;
  }

  if (now - lastEdgeMs_ >= debounceMs_ && raw != stableState_) {
    stableState_ = raw;
    if (stableState_ == LOW) {
      // Pressed.
      pressStartMs_ = now;
      longPressFired_ = false;
    } else {
      // Released.
      if (!longPressFired_ && onClick_) {
        onClick_();
      }
    }
  }

  if (stableState_ == LOW && !longPressFired_ &&
      now - pressStartMs_ >= longPressMs_) {
    longPressFired_ = true;
    if (onLongPress_) {
      onLongPress_();
    }
  }
}
