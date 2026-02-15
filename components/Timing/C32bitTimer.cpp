#include "C32bitTimer.h"
#include "Setup.h"

C32bitTimer::C32bitTimer() : CTimerBase(), _period(0), _lastMarker(0), _nextMarker(0) { }

C32bitTimer C32bitTimer::From_uS(double uS) {
  C32bitTimer marker;
  marker._period = static_cast<uint32_t>(std::ceil(uS / CTimerBase::getMicrosecondsPerTick()));
  marker.reset();
  return marker;
}

C32bitTimer C32bitTimer::From_mS(double mS) {
  C32bitTimer marker;
  marker._period = static_cast<uint32_t>(std::ceil(mS / CTimerBase::getMillisecondsPerTick()));
  marker.reset();
  return marker;
}

C32bitTimer C32bitTimer::From_S (double  S) {
  C32bitTimer marker;
  marker._period = static_cast<uint32_t>(std::ceil( S / CTimerBase::getSecondsPerTick()));
  marker.reset();
  return marker;
}

C32bitTimer C32bitTimer::From_Hz(double Hz) {
  C32bitTimer marker;
  marker._period = static_cast<uint32_t>(std::ceil(1.0/ (CTimerBase::getSecondsPerTick() * Hz)));
  marker.reset();
  return marker;
}

void C32bitTimer::A2DWait() const {
  while (true) {
    uint32_t now = ARM_DWT_CYCCNT;
    int32_t diff = static_cast<int32_t>(now - _nextMarker);
    if (diff >= 0) break;
    A2D.poll();
  }
  _nextMarker += _period;
}


double C32bitTimer::getRemaining_S() const {
  uint32_t now = ARM_DWT_CYCCNT;
  if (_period == 0) return 0.0;
  int32_t diff = static_cast<int32_t>(_nextMarker - now);  // signed
  if (diff <= 0) return 0.0;
  return diff * CTimerBase::getSecondsPerTick();
}

double C32bitTimer::getRemaining_mS() const { return getRemaining_S() * 1'000.0;     }
double C32bitTimer::getRemaining_uS() const { return getRemaining_S() * 1'000'000.0; }
