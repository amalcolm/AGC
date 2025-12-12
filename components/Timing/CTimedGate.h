#pragma once
#include <cstdint>
#include "CTimer.h"
#include "Helpers.h"

class CTimedGate {
  public:
    CTimedGate(double period) {
      _period = static_cast<uint64_t>(period / CTimer::getSecondsPerTick());
      nextTime = CTimer::time() + _period;
    }

  public:
    bool notDue() { 
      if (CTimer::time() < nextTime) return true;
      nextTime += _period;
      return false;
    }

  private:
    uint64_t _period;
    uint64_t nextTime;
};