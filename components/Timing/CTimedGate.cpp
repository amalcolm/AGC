#include "CTimedGate.h"
#include "CTimer.h"
#include "Setup.h"
#include "CA2D.h"

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

CA2DTimedGate::CA2DTimedGate(double period) : CTimedGate(period) {}  // default 250 Hz

bool CA2DTimedGate::block() {
  passed |= CTimedGate::pass();

  bool result = !passed || A2D.isBusy();
  if (!result) passed = false;
  return result;
}

bool CA2DTimedGate::pass() {
   passed |= CTimedGate::pass();

   bool result = passed && !A2D.isBusy();
   if (result) passed = false;
   return result;
}