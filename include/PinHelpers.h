#pragma once
#include <Arduino.h>


// A wrapper for a single digital output pin
template <int PIN>
struct OutputPin {
  void init(int mode = OUTPUT) const { pinMode(PIN, OUTPUT);                 }
  void write(int value)        const { digitalWrite(PIN, value);             }
  void toggle()                const { digitalWrite(PIN, !digitalRead(PIN)); }
  int  getNum()                const { return PIN;                           }
};

// A wrapper for a single digital input pin
template <int PIN>
struct InputPin {
  void init(int mode = INPUT)  const { pinMode(PIN, mode); }
  int read()                   const { return digitalRead(PIN); }
  int getNum()                 const { return PIN; }
};


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
