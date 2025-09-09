#pragma once
#include <Arduino.h>


// A wrapper for a single digital output pin
template <int PIN>
struct OutputPin {
  void init(int mode = OUTPUT) { pinMode(PIN, mode);  clear();               }
  void write(int level)        { digitalWriteFast(PIN, _value = level);      }
  void toggle()                { digitalWriteFast(PIN, !digitalRead(PIN));   }
  void set()                   { digitalWriteFast(PIN, _value = HIGH);       }
  void clear()                 { digitalWriteFast(PIN, _value = LOW);        }
  
  constexpr operator int() const noexcept { return PIN; }
  uint8_t _value;
};

// A wrapper for a single digital input pin
template <int PIN>
struct InputPin {
  void init(int mode = INPUT)  const { pinMode(PIN, mode);      }
  int read()                   const { return digitalRead(PIN); }
  
  constexpr operator int() const noexcept { return PIN; }
};


// Manages a contiguous range of LED output pins
template <int START_PIN, int END_PIN>
struct LedPinRange {
  void init() const {
    for (int pin = START_PIN; pin <= END_PIN; ++pin) {
      pinMode(pin, OUTPUT);
      digitalWrite(pin, LOW); // Default to off
    }
  }

  void clear() const {
    for (int pin = START_PIN; pin <= END_PIN; ++pin) {
      digitalWrite(pin, LOW);
    }
  }
};
