#pragma once
#include "Helpers.h"
#include <Arduino.h>
#include <array>
#include <vector>


// -- Base ----------------------------------------------------------
struct Pins {
protected:
  static constexpr int MAX_HEADS = 16;
  static constexpr int MAX_PINS  = 42;
  inline static int numHeadsUsed = 0;
  inline static std::array<std::array<Pins*, MAX_PINS>, MAX_HEADS> pinMap{};

  inline static int _currentHead = -1; // chosen head to use

  int _pin = 255; // invalid pin number

  explicit Pins(std::initializer_list<uint32_t> pinPerHead ) {
    int iHead = 0;
    for (auto pin : pinPerHead) { 
      if (iHead >= MAX_HEADS) ERROR("Too many heads: %d (max %d)", iHead, MAX_HEADS);
      if (pin >= MAX_PINS) ERROR("Pin %u out of range (head %d, max %d)", pin, iHead, MAX_PINS);

      if (iHead >= numHeadsUsed)
        numHeadsUsed = iHead + 1;

      if (pinMap[iHead][pin] != nullptr) 
        ERROR("Pin %u already defined. Head %d", pin, iHead);
      else
        pinMap[iHead][pin] = this;    


      ++iHead;
    }
  }


public:
  enum class Kind { Output, Input };
  Pins(const Pins&) = delete;
  Pins& operator=(const Pins&) = delete;

  virtual Kind kind() const noexcept = 0; 
  constexpr operator int() const noexcept { return _pin; }
  int getNum() const noexcept { return _pin; }

  static Pins* get(int pin) {
    if (_currentHead < 0 || pin < 0 || pin >= MAX_PINS) return nullptr;

    return pinMap[_currentHead][pin];
  }

  static void setHead(int head) {
    if (head < 0 || head >= numHeadsUsed) ERROR("Head %d out of range", head);
    if (head == _currentHead) return;

    if (head != 0)
      for (int pin = 0; pin < MAX_PINS; ++pin) {
        if (auto* p = pinMap[0][pin])
          if (pinMap[head][pin] == nullptr)
            pinMap[head][pin] = p;
      }    

    for (int pin = 0; pin < MAX_PINS; ++pin)
      if (auto* p = pinMap[head][pin]) p->_pin = pin;

    _currentHead = head;
  }
  
};

// -- Output --------------------------------------------------------
struct OutputPin : Pins {
  explicit OutputPin(std::initializer_list<uint32_t> pinPerHead) : Pins(pinPerHead) {}
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
  explicit InputPin(std::initializer_list<uint32_t> pinPerHead) : Pins(pinPerHead) {}
  Kind kind() const noexcept override { return Kind::Input; }

  void init(int mode = INPUT) const { pinMode(_pin, mode); }
  int  read() const                 { return digitalRead(_pin); }
};

// -- Range of predefined Output Pins ------------------------------
struct LedPinRange {
  std::vector<OutputPin*> pins;
  bool inverted = false;

  LedPinRange(int startPin, int endPin) {
    if (startPin > endPin) std::swap(startPin, endPin);
    pins.reserve(endPin - startPin + 1);
    
    for (int p = startPin; p <= endPin; ++p) {
      Pins* base = Pins::get(p);
      if (!base || base->kind() != Pins::Kind::Output) ERROR("Pin %d not defined as OutputPin", p);
      pins.emplace_back(static_cast<OutputPin*>(base));
    }
  }

  LedPinRange& init()  { for (auto* pin : pins) { pin->init(); pin->clear(); } return *this; }
  LedPinRange& set()   { for (auto* pin : pins) pin->set();   return *this; }
  LedPinRange& clear() { for (auto* pin : pins) pin->clear(); return *this; }
  LedPinRange& invert(){ for (auto* pin : pins) pin->invert(); inverted = !inverted; return *this; }
};
