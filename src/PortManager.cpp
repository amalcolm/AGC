// PortManager.h (or .hpp)
#pragma once
#include <Arduino.h>
#include <vector>
#include <stdint.h>
#include <initializer_list>

class PortManager {
public:
  // Construct with a list of Teensy pin numbers for LEDs
  PortManager(std::initializer_list<uint8_t> pins)
    : _pins(pins)
  {
    init();
  }

  PortManager(const std::vector<uint8_t>& pins)
    : _pins(pins)
  {
    init();
  }

  // Number of logical LEDs
  size_t count() const { return _pins.size(); }

  // Set all LEDs to on/off (true = HIGH, false = LOW)
  void setAll(bool on) {
    uint64_t newState = on ? ((count() >= 64) ? ~uint64_t{0} : ((uint64_t{1} << count()) - 1)) : 0;
    write(newState);
  }

  // Set by bitmask of logical indices (bit i controls _pins[i])
  void write(uint64_t newState) {
    // Diff with cached state to avoid unnecessary writes
    uint64_t diff = _state ^ newState;
    if (!diff) return;

    // Accumulate per-port set/clear masks
    for (auto& g : _groups) { g.setMask = 0; g.clearMask = 0; }

    // Walk LEDs and build masks only for pins that actually changed
    for (size_t i = 0; i < _pins.size(); ++i) {
      uint64_t bit = (uint64_t{1} << i);
      if ((diff & bit) == 0) continue; // unchanged

      const auto& p = _pinInfo[i];
      if (newState & bit) {
        _groups[p.groupIndex].setMask |= p.hwMask;
      } else {
        _groups[p.groupIndex].clearMask |= p.hwMask;
      }
    }

    // Now perform at most two writes per hardware port: SET and CLEAR
    for (auto& g : _groups) {
      if (g.setMask)   *g.setReg   = g.setMask;
      if (g.clearMask) *g.clearReg = g.clearMask;
    }

    _state = newState;
  }

  // Convenience helpers: set individual logical indices
  void setOn(std::initializer_list<size_t> indices)  { applyIndices(indices, /*on=*/true);  }
  void setOff(std::initializer_list<size_t> indices) { applyIndices(indices, /*on=*/false); }
  void toggle(std::initializer_list<size_t> indices) {
    uint64_t ns = _state;
    for (auto i : indices) if (i < _pins.size()) ns ^= (uint64_t{1} << i);
    write(ns);
  }

  // Get the cached state (bit i -> _pins[i])
  uint64_t state() const { return _state; }

private:
  struct Group {
    volatile uint32_t* setReg;   // portSetRegister(anyPinInGroup)
    volatile uint32_t* clearReg; // portClearRegister(anyPinInGroup)
    volatile uint32_t* toggleReg;// portToggleRegister(anyPinInGroup) (not used in write(), but kept for completeness)
    uint32_t setMask  = 0;
    uint32_t clearMask= 0;
  };

  struct PinInfo {
    size_t groupIndex;       // index into _groups
    uint32_t hwMask;         // digitalPinToBitMask(pin)
  };

  std::vector<uint8_t> _pins;
  std::vector<Group>   _groups;
  std::vector<PinInfo> _pinInfo;
  uint64_t             _state = 0; // cached logical state

  void init() {
    _groups.clear();
    _pinInfo.resize(_pins.size());

    // Group by actual hardware port registers discovered at runtime
    for (size_t i = 0; i < _pins.size(); ++i) {
      uint8_t pin = _pins[i];
      pinMode(pin, OUTPUT);

      volatile uint32_t* set = portSetRegister(pin);
      volatile uint32_t* clr = portClearRegister(pin);
      volatile uint32_t* tgl = portToggleRegister(pin);
      uint32_t mask = digitalPinToBitMask(pin);

      // Find or create group for this port
      size_t gi = SIZE_MAX;
      for (size_t g = 0; g < _groups.size(); ++g) {
        if (_groups[g].setReg == set) { gi = g; break; }
      }
      if (gi == SIZE_MAX) {
        Group ng{ set, clr, tgl, 0, 0 };
        _groups.push_back(ng);
        gi = _groups.size() - 1;
      }

      _pinInfo[i] = PinInfo{ gi, mask };
    }
  }

  void applyIndices(std::initializer_list<size_t> indices, bool on) {
    uint64_t ns = _state;
    for (auto i : indices) {
      if (i < _pins.size()) {
        if (on) ns |=  (uint64_t{1} << i);
        else    ns &= ~(uint64_t{1} << i);
      }
    }
    write(ns);
  }
};
