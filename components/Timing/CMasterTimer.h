#pragma once
#include "CTimer.h"
class CMasterTimer : public CTimer {
  
private:
  inline static uint64_t s_connectTime = 0;
         static uint32_t s_loopTicks;       // note; 32bit for use with ARM_DWT_CYCCNT, not elapsed();

  uint32_t m_stateChange = 0; // ditto; wraps with ARM_DWT_CYCCNT



public:
  CMasterTimer();

  inline void   restartConnectTiming() { s_connectTime = CTimer::time();                                         }
  inline double getConnectTime()       { return (CTimer::time() - s_connectTime) * CTimerBase::s_SecondsPerTick; }

  inline static double upTime() { return                   CTimer::time() * CTimerBase::s_SecondsPerTick; }
  inline double getStateTime()  { return (ARM_DWT_CYCCNT - m_stateChange) * CTimerBase::s_SecondsPerTick; }

  void markStateChange(); // Sets m_stateChange and aligns A2D read timing
  
  inline bool isProcessingState() {
    uint32_t stateTicks = ARM_DWT_CYCCNT - m_stateChange;
    if (stateTicks < s_loopTicks) return true;

    m_stateChange += s_loopTicks; 
    return false;
  }

  void Delay_uS(uint32_t microseconds);

  
private:
  void callibrate();
};
