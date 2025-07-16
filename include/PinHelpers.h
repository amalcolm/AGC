#pragma once
#include <Arduino.h>

// --- Template-based Pin Wrappers (to reduce boilerplate) ---

// A wrapper for a single digital output pin
template <int PIN>
struct OutputPin {
  void init() const {
    pinMode(PIN, OUTPUT);
  }
  void write(int value) const {
    digitalWrite(PIN, value);
  }
  void toggle() const {
    digitalWrite(PIN, !digitalRead(PIN));
  }
};

// A wrapper for a single digital input pin
template <int PIN>
struct InputPin {
  void init(int mode = INPUT) const {
    pinMode(PIN, mode);
  }
  int read() const {
    return digitalRead(PIN);
  }
};

// --- Functional Pin Grouping Classes ---

// Manages a contiguous range of LED output pins
struct LedPinRange {
  const int _startPin, _endPin;
public:
  LedPinRange(int start, int end) : _startPin(start), _endPin(end) {}

  void init() const {
    for (int pin = _startPin; pin <= _endPin; ++pin) {
      pinMode(pin, OUTPUT);
      digitalWrite(pin, HIGH); // Default to off
    }
  }

  void deactivate() const {
    for (int pin = _startPin; pin <= _endPin; ++pin) {
      digitalWrite(pin, HIGH);
    }
  }
};
