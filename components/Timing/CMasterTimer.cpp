#include "CMasterTimer.h"
#include "Setup.h"
#include "CA2D.h"
#include <cstdint>

uint32_t   HW_DELAY_TICKS = CTimerBase::microsecondsToTicks(CFG::POT_UPDATE_OFFSET_uS + CFG::HEAD_SETTLE_TIME_uS);

uint32_t HEAD_DELAY_TICKS = CTimerBase::microsecondsToTicks(CFG::HEAD_SETTLE_TIME_uS);

uint32_t A2D_PERIOD_TICKS = CTimerBase::  hzToPeriodInTicks(CFG::A2D_READING_SPEED_Hz);


uint32_t A2D_DELAY_TICKS = HEAD_DELAY_TICKS + A2D_PERIOD_TICKS * 2/6;

CMasterTimer::CMasterTimer() : CTimer() { }

void CMasterTimer::markStateChange() { 
  uint32_t now = ARM_DWT_CYCCNT;
  state.reset();
     HW.resetAt(now + HW_DELAY_TICKS); // align A2D read timing
   Head.resetAt(now + HEAD_DELAY_TICKS);
    A2D.resetAt(now + A2D_DELAY_TICKS);
}

bool CMasterTimer::addEvent(const enum EventKind kind, double stateTime) {
  if (stateTime < 0) stateTime = state.getSeconds();
  
  CA2D* pA2D = &::A2D;  // get singleton from global to avoid conflict with member name A2D

  return pA2D->m_pBlockToFill->tryAddEvent(kind, stateTime);
  
}
