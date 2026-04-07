#pragma once
#include <Arduino.h>
#include "config.h"

class Button {
public:
  Button();
  void begin();
  void update();

  bool wasPressed();        // Single tap — fires once
  bool wasHeld();           // Hold >= HOLD_MS — fires once
  bool isCurrentlyHeld();   // Raw held state
  float holdProgress();     // 0.0–1.0 for hold bar

private:
  bool          _last;
  bool          _current;
  bool          _pressFlag;
  bool          _holdFlag;
  bool          _holdFired;
  unsigned long _debounceTime;
  unsigned long _pressTime;
};
