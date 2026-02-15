#pragma once
#include "C32bitTimer.h"
#include "CA2DTimer.h"
#include "CTimer.h"

class CMasterTimer : public CTimer {
  
private:
  inline static uint64_t s_connectTime = 0;

  double m_maxPollDuration = 0.0;

public:
  const C32bitTimer state = C32bitTimer::From_uS(CFG::STATE_DURATION_uS     ).setPeriodic(true);
  
  const C32bitTimer Head  = C32bitTimer::From_uS(CFG::HEAD_SETTLE_TIME_uS   ).setPeriodic(false);
  const C32bitTimer HW    = C32bitTimer::From_uS(CFG::POT_UPDATE_OFFSET_uS  ).setPeriodic(false);

        CA2DTimer   A2D   = CA2DTimer{};

public:
  CMasterTimer();
  
  inline void   setConnectTime() { s_connectTime = CTimer::time();                                         }
  inline double getConnectTime() { return (CTimer::time() - s_connectTime) * CTimerBase::s_SecondsPerTick; }

  inline static double upTime() { return CTimer::time() * CTimerBase::s_SecondsPerTick; }
  inline double getStateTime()  { return state.getSeconds(); }
  inline double getStateTime(uint32_t now)  { return state.getSeconds(now); }

  void markStateChange(); // Sets m_stateChange and aligns A2D read timing

  bool addEvent(const enum EventKind kind, double time = -1.0);


  inline void   setPollDuration(double duration) { if (duration > m_maxPollDuration) m_maxPollDuration = duration; }
  inline double getPollDuration() const          { return m_maxPollDuration; }

};
