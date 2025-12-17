#include "CTimedGate.h"
#include "CTimer.h"

CTimedGate::CTimedGate(double period) {
  _period = static_cast<uint64_t>(period / CTimer::getSecondsPerTick());
  nextTime = CTimer::time() + _period;
}

bool CTimedGate::block() { 
  if (CTimer::time() < nextTime) return true;
  nextTime += _period;
  return false;
}

bool CTimedGate::pass() { 
  if (CTimer::time() < nextTime) return false;
  nextTime += _period;
  return true;
}
