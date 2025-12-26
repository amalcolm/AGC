#pragma once
#include "CTimer.h"
class CMasterTimer : public CTimer {
  
private:
  inline static uint64_t s_connectTime = 0;

  uint32_t m_loopTicks = 0;  // note; 32bit for use with ARM_DWT_CYCCNT, not elapsed();
  uint32_t m_stateChange = 0; // ditto; wraps with ARM_DWT_CYCCNT



public:
  CMasterTimer();


  void setPeriodTicks(uint64_t   ticks);
  void setPeriodUS   (int microseconds);
  void setPeriodMS   (int milliseconds);
  void setPeriod     (double   seconds);

  inline void   restartConnectTiming() { s_connectTime = CTimer::time();                                         }
  inline double getConnectTime()       { return (CTimer::time() - s_connectTime) * CTimerBase::s_SecondsPerTick; }

  inline static double upTime() { return                   CTimer::time() * CTimerBase::s_SecondsPerTick; }
  inline double getStateTime()  { return (ARM_DWT_CYCCNT - m_stateChange) * CTimerBase::s_SecondsPerTick; }

  void markStateChange(); // Sets m_stateChange and aligns A2D read timing
  
  inline bool isProcessingState() {
    uint32_t stateTicks = ARM_DWT_CYCCNT - m_stateChange;
    if (stateTicks >= m_loopTicks) {
      m_stateChange += m_loopTicks; 
      return false;
    }
    return true;
  }

  
private:
  void callibrate();
};
