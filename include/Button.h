#pragma once

#include <Arduino.h>
#include <functional>

// Non-blocking button handler for an active-low button wired with the
// internal pull-up (pin reads LOW while pressed). Call loop() frequently
// from the main loop(); it debounces the input and fires onClick on a
// quick press-release, or onLongPress once the hold exceeds longPressMs
// (onClick is then suppressed for that press).
class Button {
 public:
  explicit Button(uint8_t pin, unsigned long longPressMs = 400,
                   unsigned long debounceMs = 30);

  void begin();
  void loop();

  void onClick(std::function<void()> callback) { onClick_ = callback; }
  void onLongPress(std::function<void()> callback) { onLongPress_ = callback; }

 private:
  uint8_t pin_;
  unsigned long longPressMs_;
  unsigned long debounceMs_;

  int stableState_;
  int lastRawState_;
  unsigned long lastEdgeMs_;
  unsigned long pressStartMs_;
  bool longPressFired_;

  std::function<void()> onClick_;
  std::function<void()> onLongPress_;
};
