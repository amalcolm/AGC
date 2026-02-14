#include "C32bitA2DTimer.h"
#include "Config.h"
#include "Setup.h"
#include "CA2D.h"

C32bitA2DTimer::C32bitA2DTimer() : C32bitTimer() { }

void C32bitA2DTimer::reset(uint32_t start, uint32_t period) {
  _lastMarker = start;
  _nextMarker = start + period;
  _period = period;
}