#include "CMasterTimer.h"
#include "Hardware.h"
#include "../teensy_compat.h"

uint32_t CMasterTimer::s_loopTicks = (uint32_t)std::ceil((double)CFG::LoopPeriod_uS / CTimerBase::s_MicrosecondsPerTick);

CMasterTimer::CMasterTimer() : CTimer() { }



void   CMasterTimer::markStateChange() { 
  m_stateChange = ARM_DWT_CYCCNT; 
  Hardware::gate.setNextTime(Timer.elapsed());
}

bool first = true;
void CMasterTimer::Delay_uS(uint32_t us) {
  static constexpr uint32_t yieldThreshold = 2000; // ~3.3us at 600MHz
  if (us <= 0) return;
  if (s_loopTicks == 0 || m_stateChange == 0) return; // santity check; timer not started yet
  if (first) 
    USB.printf("delay uS called with %u us.  m_loopTime=%llf\n", us, s_loopTicks * CTimer::getMicrosecondsPerTick());

  uint32_t start = ARM_DWT_CYCCNT;
  uint32_t ticks = (uint32_t)std::ceil((double)us / CTimerBase::s_MicrosecondsPerTick);

  while ((uint32_t)(ARM_DWT_CYCCNT - start) < ticks ) {

    uint32_t stateTicks = ARM_DWT_CYCCNT - m_stateChange;
    if (stateTicks > s_loopTicks) break;
    
    if (ticks > yieldThreshold) 
      yield();
    else
      __asm__ volatile("nop");
  }

  first = false;
}
