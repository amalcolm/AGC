#pragma once
#include <Arduino.h>


// A wrapper for a single digital output pin
template <int PIN>
struct OutputPin {
  void init(int mode = OUTPUT) { pinMode(PIN, mode);  clear();              }
  void write(int level)        { digitalWriteFast(PIN, value = level);      }
  void toggle()                { digitalWriteFast(PIN, !digitalRead(PIN));  }
  void set()                   { digitalWriteFast(PIN, value = HIGH);       }
  void clear()                 { digitalWriteFast(PIN, value = LOW);        }
  
  constexpr operator int() const noexcept { return PIN; }
  uint8_t value;
};

// A wrapper for a single digital input pin
template <int PIN>
struct InputPin {
  void init(int mode = INPUT)  const { pinMode(PIN, mode); }
  int read()                   const { return digitalRead(PIN); }
  
  constexpr operator int() const noexcept { return PIN; }
};


// Manages a contiguous range of LED output pins
struct LedPinRange {
  const int _startPin, _endPin;
public:
  LedPinRange(int start, int end) : _startPin(start), _endPin(end) {}

  void init() const {
    for (int pin = _startPin; pin <= _endPin; ++pin) {
      pinMode(pin, OUTPUT);
      digitalWrite(pin, LOW); // Default to off
    }
  }

  void deactivate() const {
    for (int pin = _startPin; pin <= _endPin; ++pin) {
      digitalWrite(pin, LOW);
    }
  }
};
