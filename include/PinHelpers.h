#pragma once
#include "Helpers.h"
#include <Arduino.h>
#include <unordered_map>
#include <vector>

// -- Base ----------------------------------------------------------
struct Pins {
protected:
  inline static std::unordered_map<int, Pins*> pinMap;
  const int _pin;
  explicit Pins(int pin) : _pin(pin) {
    if (pinMap.count(pin)) error("Pin %d already in use!", pin);
    pinMap.emplace(pin, this);
  }

public:
  enum class Kind { Output, Input };
  virtual ~Pins() {
    auto it = pinMap.find(_pin);
    if (it != pinMap.end() && it->second == this) pinMap.erase(it);
  }
  Pins(const Pins&) = delete;
  Pins& operator=(const Pins&) = delete;

  virtual Kind kind() const noexcept = 0; 
  constexpr operator int() const noexcept { return _pin; }
  int getNum() const noexcept { return _pin; }

  static Pins* get(int pin) {
    auto it = pinMap.find(pin);
    return (it == pinMap.end()) ? nullptr : it->second;
  }
};

// -- Output --------------------------------------------------------
struct OutputPin : Pins {
  explicit OutputPin(int pin) : Pins(pin) {}
  Kind kind() const noexcept override { return Kind::Output; }

  void init(int mode = OUTPUT) { pinMode(_pin, mode); clear(); }
  void write(int level)        { digitalWrite(_pin, level); }
  void toggle()                { digitalWrite(_pin, !digitalRead(_pin)); }
  void set()                   { digitalWrite(_pin, _high); }
  void clear()                 { digitalWrite(_pin, _low); }
  OutputPin& invert()          { uint8_t t=_high; _high=_low; _low=t; return *this; }

private:
  uint8_t _high = HIGH, _low = LOW;
};

// -- Input ---------------------------------------------------------
struct InputPin : Pins {
  explicit InputPin(int pin) : Pins(pin) {}
  Kind kind() const noexcept override { return Kind::Input; }

  void init(int mode = INPUT) const { pinMode(_pin, mode); }
  int  read() const                 { return digitalRead(_pin); }
};

// -- Range of predefined Output Pins ------------------------------
struct LedPinRange {
  std::vector<OutputPin*> pins;
  bool inverted = false;

  LedPinRange(int startPin, int endPin) {
    pins.reserve(endPin - startPin + 1);
    for (int p = startPin; p <= endPin; ++p) {
      Pins* base = Pins::get(p);
      if (!base || base->kind() != Pins::Kind::Output) error("Pin %d not defined as OutputPin", p);
      pins.emplace_back(static_cast<OutputPin*>(base));
    }
  }

  LedPinRange& init()  { for (auto* pin : pins) { pin->init(); pin->clear(); } return *this; }
  LedPinRange& set()   { for (auto* pin : pins) pin->set();   return *this; }
  LedPinRange& clear() { for (auto* pin : pins) pin->clear(); return *this; }
  LedPinRange& invert(){ for (auto* pin : pins) pin->invert(); inverted = !inverted; return *this; }
};
