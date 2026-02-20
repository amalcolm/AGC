#pragma once
#include <cstdint>
#include "Arduino.h"
#include "CTimerBase.h"
#include "Helpers.h"

class CSafeTimer {
private:
  uint32_t m_startTicks;
  uint32_t m_periodTicks = 0;

public:
  CSafeTimer() : m_startTicks(ARM_DWT_CYCCNT) { }

  inline uint32_t getStartTicks() const { return m_startTicks; }
  inline uint32_t getPeriodTicks() const { return m_periodTicks; }
  inline uint32_t elapsedTicks() { return ARM_DWT_CYCCNT - m_startTicks; }

  inline void reset()               { m_startTicks = ARM_DWT_CYCCNT; }
  inline void reset(uint32_t start) { m_startTicks = start; }

  inline void reset(uint32_t start, uint32_t period) {
    m_startTicks = start;
    m_periodTicks = period;
  }
  

  inline double getRemaining_S() const {
    const uint32_t now = ARM_DWT_CYCCNT;
    if (m_periodTicks == 0) return 0.0;
    
    const uint32_t end = m_startTicks + m_periodTicks; // wraps naturally (good)

    // Wrap-safe signed delta as long as true delta magnitude < 2^31 ticks (~3 seconds)
    const int32_t ticksRemaining = static_cast<int32_t>(end - now);

    if (ticksRemaining <= 0) return 0.0;

    return static_cast<double>(ticksRemaining) * CTimerBase::getSecondsPerTick();
  }
};