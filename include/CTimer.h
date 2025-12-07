#pragma once
#include <cstdint>
#include "Arduino.h"

class CTimer {
  
private:
   
  uint64_t startTime;
  uint64_t calibration;
  double ticksPerSecond;
  double ticksPerMS;
  double ticksPerUS;

  static volatile uint64_t s_connectTime;
  static volatile uint32_t s_lastReading;
  static volatile uint64_t s_overflowCount;

public:
  CTimer();

  inline static uint64_t time() {
    static const uint64_t increment = 0x1ULL << 32;

    uint32_t current = ARM_DWT_CYCCNT;

    if (current < s_lastReading) {
      __disable_irq();
        uint64_t val = s_overflowCount + increment;
        s_overflowCount = val;
      __enable_irq();
    }

    s_lastReading = current;

  return s_overflowCount + current;
}   

  inline void     restart()  { startTime = time();            }
  inline uint64_t elapsed()  { return time() - startTime;     }
  inline double   mS()       { return elapsed() / ticksPerMS; }
  inline double   uS()       { return elapsed() / ticksPerUS; }


  inline void     restartConnectTiming() { s_connectTime = time();                           }
  inline double   getConnectTime()       { return (time() - s_connectTime) / ticksPerSecond; }

  static void     gpt1Handler();
  
private:
  inline uint64_t _raw() const { return ARM_DWT_CYCCNT; }

  
  void initGPT1();
};
