#pragma once
#include "CTimedGate.h"
#include <cstdint>

class CDataTimedGate : public CTimedGate {
  const int32_t INACTIVE = -1;  // must be negative

public:
  CDataTimedGate(double period, uint32_t postDataLoops);

  // Notify that new data has arrived
  inline void notifyData() {
    _haveData = true;
  }

  // Called once per loop; returns true when gate passes *and* delay is done
  bool pass();

  // Optional: reset the gate state manually
  inline void reset()  {
    _gatePassed = false;
    _haveData = false;
    _delayCounter = INACTIVE;
  }

private:
  uint32_t _postDataLoops;    // how long to wait after data
   int32_t _delayCounter;  // countdown timer
  bool _haveData;             // whether new data has arrived since last pass
  bool _gatePassed;           // latch for gate event
};
