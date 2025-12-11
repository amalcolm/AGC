#pragma once
#include <cstdint>
#include "CTimer.h"


class CTimedGate {
  public:
    CTimedGate(double period) {
      _period = static_cast<uint64_t>(period * CTimer::getTicksPerSecond());
      lastTime = CTimer::time();
    }

  public:
    bool notDue() { 
      uint64_t now = CTimer::time();
      if (now < lastTime + _period) return true;
      lastTime += _period;
      return false;
    }

  private:
    uint64_t _period;
    uint64_t lastTime;
};