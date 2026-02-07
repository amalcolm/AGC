#pragma once
#include "CMarkers.h"

class CMasterTimer : public CTimer {
  
private:
  inline static uint64_t s_connectTime = 0;

public:
  const CMarker32 state = CMarker32::From_uS(CFG::STATE_DURATION_uS     ).setPeriodic(true);
  const CMarker32 A2D   = CMarker32::From_uS(CFG::A2D_READING_PERIOD_uS ).setPeriodic(true);
  
  const CMarker32 Head  = CMarker32::From_uS(CFG::HEAD_SETTLE_TIME_uS   ).setPeriodic(false);
  const CMarker32 HW    = CMarker32::From_uS(CFG::POT_UPDATE_OFFSET_uS  ).setPeriodic(false);

public:
  CMasterTimer();

  inline void   restartConnectTiming() { s_connectTime = CTimer::time();                                         }
  inline double getConnectTime()       { return (CTimer::time() - s_connectTime) * CTimerBase::s_SecondsPerTick; }

  inline static double upTime() { return CTimer::time() * CTimerBase::s_SecondsPerTick; }
  inline double getStateTime()  { return state.getSeconds(); }

  void markStateChange(); // Sets m_stateChange and aligns A2D read timing

  bool addEvent(const enum EventKind kind, double time = -1.0);

};
