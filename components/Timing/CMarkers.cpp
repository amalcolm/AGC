#include "CMarkers.h"
#include "Setup.h"

CMarker32::CMarker32() : _period(0), _nextMarker(0) { }

CMarker32 CMarker32::From_uS(double uS) {
  CMarker32 marker;
  marker._period = static_cast<uint32_t>(std::ceil(uS / CTimerBase::getMicrosecondsPerTick()));
  marker.reset();
  return marker;
}

CMarker32 CMarker32::From_mS(double mS) {
  CMarker32 marker;
  marker._period = static_cast<uint32_t>(std::ceil(mS / CTimerBase::getMillisecondsPerTick()));
  marker.reset();
  return marker;
}

CMarker32 CMarker32::From_S (double S) {
  CMarker32 marker;
  marker._period = static_cast<uint32_t>(std::ceil(S / CTimerBase::getSecondsPerTick()));
  marker.reset();
  return marker;
}

CMarker32 CMarker32::From_Hz(double Hz) {
  CMarker32 marker;
  marker._period = static_cast<uint32_t>(std::ceil(1.0 / (CTimerBase::getSecondsPerTick() * Hz)));
  marker.reset();
  return marker;
}

void CMarker32::A2DWait() const {
  while (true) {
    uint32_t now = ARM_DWT_CYCCNT;
    int32_t diff = static_cast<int32_t>(now - _nextMarker);
    if (diff >= 0) break;
    A2D.poll();
  }
  _nextMarker += _period;
}


// CMarkers.h
double CMarker32::getRemaining_S() const {
  if (_period == 0) return 0.0;
  uint32_t now = ARM_DWT_CYCCNT;
  int32_t diff = static_cast<int32_t>(_nextMarker - now);  // signed
  if (diff <= 0) return 0.0;
  return diff * CTimerBase::getSecondsPerTick();
}

double CMarker32::getRemaining_mS() const { return getRemaining_S() * 1'000.0;     }
double CMarker32::getRemaining_uS() const { return getRemaining_S() * 1'000'000.0; }
