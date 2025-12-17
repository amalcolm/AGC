#include "CDataTimedGate.h"


CDataTimedGate::CDataTimedGate(double period, uint32_t postDataLoops)
  : CTimedGate(period),
    _postDataLoops(postDataLoops),
    _delayCounter(INACTIVE),
    _haveData(false),
    _gatePassed(false) {}


bool CDataTimedGate::pass() {
  // Step 1: if the gate period has elapsed, mark it
  if (CTimedGate::pass())
    _gatePassed = true;

  // Step 2: handle post-data timing
  if (_haveData) {
    _delayCounter = _postDataLoops;
    _haveData = false;
  }
  else
  if (_delayCounter > 0) {
    _delayCounter--;
  }

  // Step 3: only allow a "pass" when the gate has passed AND delay is done
  if (_gatePassed && _delayCounter == 0) {
    reset();
    return true;
  }

  return false;
}
