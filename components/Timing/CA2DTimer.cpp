#include "CA2DTimer.h"
#include "Config.h"
#include "Setup.h"
#include "CA2D.h"

CA2DTimer::CA2DTimer() : C32bitTimer() { }

void CA2DTimer::reset(uint32_t start, uint32_t period) {
  _lastMarker = start;
  _nextMarker = start + period;
  _period = period;
}