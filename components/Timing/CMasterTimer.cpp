#include "CMasterTimer.h"
#include <cstdint>

uint32_t CMasterTimer::s_readPeriod = (uint32_t)std::ceil(1.0 / (CFG::A2D_READING_SPEED_Hz * CTimerBase::s_SecondsPerTick));

uint32_t HW_DELAY_TICKS = (uint32_t)std::ceil(
  (CFG::POT_UPDATE_OFFSET_uS + CFG::HEAD_SETTLE_TIME_uS) / CTimerBase::getMicrosecondsPerTick());

uint32_t CMasterTimer::s_lastTick = 0;

CMasterTimer::CMasterTimer() : CTimer() { }

void CMasterTimer::markStateChange() { 
  state.reset();
  HW.resetAfter(HW_DELAY_TICKS); // align A2D read timing
}
