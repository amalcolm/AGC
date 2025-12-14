#pragma once
#include <cstdint>

class CTimedGate {
  public:
    CTimedGate(double period);

  public:
    bool notDue();
    
  private:
    uint64_t _period;
    uint64_t nextTime;
};