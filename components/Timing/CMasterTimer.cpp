#include "CMasterTimer.h"
#include "Hardware.h"
#include "../teensy_compat.h"

uint32_t CMasterTimer::s_loopTicks  = (uint32_t)std::ceil(CFG::LOOP_PERIOD_uS / CTimerBase::s_MicrosecondsPerTick);
uint32_t CMasterTimer::s_readPeriod = (uint32_t)std::ceil(1.0 / (CFG::READING_SPEED_Hz * CTimerBase::s_SecondsPerTick));

uint32_t CMasterTimer::s_lastTick = 0;

CMasterTimer::CMasterTimer() : CTimer() { }

void   CMasterTimer::markStateChange() { 
  m_stateChange = ARM_DWT_CYCCNT; 
  Hardware::gate.setNextTime(Timer.elapsed());
}

void CMasterTimer::postReadDelay() {
  
  while ((uint32_t)(ARM_DWT_CYCCNT - s_lastTick) < s_readPeriod) {

    uint32_t stateTicks = ARM_DWT_CYCCNT - m_stateChange;
    if (stateTicks > s_loopTicks) break;
    
    yield();
  }

  s_lastTick += s_readPeriod;

}
