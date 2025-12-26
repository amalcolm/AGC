#include "CMasterTimer.h"
#include "Hardware.h"
#include "../teensy_compat.h"

CMasterTimer::CMasterTimer() : CTimer() { }

void CMasterTimer::setPeriodTicks(uint64_t ticks) { 
  m_loopTicks = ticks; 
}

void  CMasterTimer::setPeriodUS(int microseconds) {
  m_loopTicks = static_cast<uint64_t>(std::round(microseconds / CTimerBase::s_MicrosecondsPerTick));
}

void   CMasterTimer::setPeriodMS(int milliseconds) {
  m_loopTicks = static_cast<uint64_t>(std::round(milliseconds / CTimerBase::s_MillisecondsPerTick));
}

void   CMasterTimer::setPeriod(double seconds) {
  m_loopTicks = static_cast<uint64_t>(std::round(seconds / CTimerBase::s_SecondsPerTick));
}


void   CMasterTimer::markStateChange() { 
  m_stateChange = ARM_DWT_CYCCNT; 
  Hardware::gate.setNextTime(Timer.elapsed());
}
