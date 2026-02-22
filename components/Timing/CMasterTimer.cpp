#include "CMasterTimer.h"
#include "Setup.h"
#include "CA2D.h"
#include <cstdint>

uint32_t   HW_DELAY_TICKS = CTimerBase::microsecondsToTicks(CFG::HEAD_SETTLE_TIME_uS + CFG::POT_UPDATE_OFFSET_uS);

uint32_t HEAD_DELAY_TICKS = CTimerBase::microsecondsToTicks(CFG::HEAD_SETTLE_TIME_uS);

uint32_t A2D_PERIOD_TICKS = CTimerBase::microsecondsToTicks(CFG::A2D_READING_PERIOD_uS);


CMasterTimer::CMasterTimer() : CTimer() { }

void CMasterTimer::markStateChange() { 
  uint32_t now = state.reset();

     HW.resetAt(now +   HW_DELAY_TICKS); // align A2D read timing
   Head.resetAt(now + HEAD_DELAY_TICKS);

    A2D.resetAt(now + HEAD_DELAY_TICKS + 1000); 
}

bool CMasterTimer::addEvent(const enum EventKind kind, double stateTime) {
  if (stateTime < 0) stateTime = state.getSeconds();
  
  CA2D* pA2D = &::A2D;  // get singleton from global to avoid conflict with member name A2D

  return pA2D->tryAddEvent(kind, stateTime);
  
}
