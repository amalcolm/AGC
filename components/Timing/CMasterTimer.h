#pragma once
#include "CMarkers.h"

class CMasterTimer : public CTimer {
  
private:
  inline static uint64_t s_connectTime = 0;

  static uint32_t s_readPeriod;
  static uint32_t s_lastTick;

public:
  const CMarker32 state = CMarker32::From_uS(CFG::STATE_DURATION_uS   );
  const CMarker32 A2D   = CMarker32::From_Hz(CFG::A2D_READING_SPEED_Hz);
  
  const CMarker32 HW    = CMarker32::From_uS(CFG::POT_UPDATE_PERIOD_uS);

public:
  CMasterTimer();

  inline void   restartConnectTiming() { s_connectTime = CTimer::time();                                         }
  inline double getConnectTime()       { return (CTimer::time() - s_connectTime) * CTimerBase::s_SecondsPerTick; }

  inline static double upTime() { return                   CTimer::time() * CTimerBase::s_SecondsPerTick; }
  inline double getStateTime()  { return state.getSeconds(); }

  void markStateChange(); // Sets m_stateChange and aligns A2D read timing

  inline void setLastTick() { s_lastTick = ARM_DWT_CYCCNT; }

};
