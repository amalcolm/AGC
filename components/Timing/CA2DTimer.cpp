#include "CA2DTimer.h"
#include "Config.h"
#include "Setup.h"
#include "CA2D.h"

CA2DTimer::CA2DTimer() : C32bitTimer() {
  _lastMarker = ARM_DWT_CYCCNT;
  _period = microsecondsToTicks(CFG::A2D_READING_PERIOD_uS);
  _nextMarker = _lastMarker + _period;

  if (CFG::A2D_USE_CONTINUOUS_MODE == false)
    setPeriodic(true);
}
/*
void CA2DTimer::reset(uint32_t start, uint32_t period) {
  _lastMarker = start;
  _nextMarker = start + period;
  _period = period;
}
*/
void CA2DTimer::sync() const {

  if (CFG::A2D_USE_CONTINUOUS_MODE) {
    // In continuous mode, we sync to the A2D data ready timing to assist with data timing within block
    A2D.waitForNextDataReady();
  }
  


}