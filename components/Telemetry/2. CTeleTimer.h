#pragma once
#include <CTelemetry.h>
#include <cstdint>
#include "Arduino.h"

class CTeleTimer : public CTelemetry {
protected:
    constexpr static uint8_t SUBGROUP = 0x01;
    inline static uint32_t instanceCounter{};

public:
  CTeleTimer(TeleGroup group = TeleGroup::TIMER, uint16_t id = 0xFFFF) : CTelemetry(group, SUBGROUP, id) {
    
    if (ID == 0xFFFF) ID = instanceCounter++;

    _registerCounter(this);
  }

protected:
  uint32_t _lastTick{};
  uint32_t _maxTick{};

public:
  inline void measure() {
    uint32_t now = ARM_DWT_CYCCNT;
    uint32_t last = _lastTick;
    _lastTick = now;

    if (_maxTick == 0) {     // first call
      _maxTick = 1;        // mark initialized
      return;
    }

    uint32_t duration = now - last;
    if (duration > _maxTick)
      _maxTick = duration;
  }

  inline uint32_t getTeleMax() {
    uint32_t val = _maxTick;
    _maxTick = 0;  // reset so next call skips first measurement again
    return val;
  }

};