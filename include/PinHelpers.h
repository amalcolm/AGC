#pragma once
#include "Helpers.h"
#include <Arduino.h>
#include <array>
#include <vector>


// -- Base ----------------------------------------------------------
struct Pins {
  enum class Kind { Input, Output, LED };

protected:
  static constexpr int MAX_HEADS = 16;
  static constexpr int MAX_PINS  = 42;
  inline static int numHeadsUsed = 0;
  inline static std::array<std::array<Pins*, MAX_PINS>, MAX_HEADS> pinMap{};

  inline static int      _currentHead = -1; // chosen head to use
  inline static uint32_t _headEpoch   = 0;  // incremented each time head is changed

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
  Pins(const Pins&) = delete;
  Pins& operator=(const Pins&) = delete;

  virtual Kind kind() const noexcept = 0; 
  constexpr operator int() const noexcept { return _pin; }
  int getNum() const noexcept { return _pin; }
  
  static Pins* get(int pin) {
    if (_currentHead < 0 || pin < 0 || pin >= MAX_PINS) return nullptr;

    return pinMap[_currentHead][pin];
  }

  static int      getCurrentHead() noexcept { return _currentHead; }
  static uint32_t getHeadEpoch()   noexcept { return _headEpoch;   }
  
  static std::array<Pins*, MAX_PINS>& getPinMap() {
    if (_currentHead < 0) ERROR("No head selected when accessing pin map");
    return pinMap[_currentHead];
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
    _headEpoch++;
  }

  static void flash(int numFlashes = 3) {

    for (int i = 0; i < numFlashes; ++i) {

      for (int pin = 24; pin < 42; pin++) digitalWrite(pin, LOW);
      delay(300);
      for (int pin = 24; pin < 42; pin++) digitalWrite(pin, HIGH);
      delay(300);

    }

    delay(1000);
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

struct LedPin : OutputPin {
    using OutputPin::OutputPin;          // inherit all OutputPin constructors
    Kind kind() const noexcept override { return Kind::LED; }
};



// -- Input ---------------------------------------------------------
struct InputPin : Pins {
  explicit InputPin(std::initializer_list<uint32_t> pinPerHead) : Pins(pinPerHead) {}
  Kind kind() const noexcept override { return Kind::Input; }

  void init(int mode = INPUT) const { pinMode(_pin, mode); }
  int  read() const                 { return digitalRead(_pin); }
};

// -- Range of predefined LED Pins ------------------------------
struct LedPinRange {
    int start{}, end{};
    uint32_t cachedEpoch{0};
    std::vector<LedPin*> pins;

    LedPinRange(int s, int e) : start(s), end(e) {
      if (start > end) std::swap(start, end);
    }

    // Call after setHead(), or before first use
    void refreshIfStale() {
        if (cachedEpoch == Pins::getHeadEpoch()) return;

        pins.clear();
        const auto& map = Pins::getPinMap();
        pins.reserve(end - start + 1);

        for (int p = start; p <= end; ++p)
          if (Pins* base = map[p]) 
            if (base->kind() == Pins::Kind::LED)
              pins.emplace_back(static_cast<LedPin*>(base));

        cachedEpoch = Pins::getHeadEpoch();
    }

    // Convenience wrappers that ensure freshness
    void init()    { refreshIfStale(); for (auto* pin : pins) { pin->init(); pin->clear(); } }
    void set()     { refreshIfStale(); for (auto* pin : pins) pin->set();    }
    void clear()   { refreshIfStale(); for (auto* pin : pins) pin->clear();  }
    void invert()  { refreshIfStale(); for (auto* pin : pins) pin->invert(); }
}; 
