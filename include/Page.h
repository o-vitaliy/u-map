#pragma once

#include <functional>

#include "AppTypes.h"

// A single screen/state of the app (e.g. "no wifi", "clock"). The app
// drives one active Page at a time: calling setup() once on activation,
// loop() every iteration while active, and buttonClick() when a button
// fires a UiAction. A page requests a switch to a different state by
// calling changeState(), which invokes the callback registered via
// stateChangeCallback().
class Page {
 public:
  virtual ~Page() = default;

  virtual void setup() = 0;
  virtual void loop() = 0;
  virtual void buttonClick(UiAction action) = 0;

  void stateChangeCallback(std::function<void(AppState)> callback) {
    stateChangeCallback_ = callback;
  }

 protected:
  void changeState(AppState newState) {
    if (stateChangeCallback_) {
      stateChangeCallback_(newState);
    }
  }

 private:
  std::function<void(AppState)> stateChangeCallback_;
};
