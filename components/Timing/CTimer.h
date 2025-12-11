#pragma once
#include <cstdint>
#include "Arduino.h"

class CTimer {
  
private:
   
  uint64_t startTime;
  static uint64_t s_calibration;
  static double s_SecondsPerTick;
  static double s_MillecondsPerTick;
  static double s_MicrosecondsPerTick;

  static volatile uint64_t s_connectTime;
  static volatile uint32_t s_lastReading;
  static volatile uint64_t s_overflowCount;

  static const uint64_t increment = 0x1ULL << 32;

public:
  CTimer();

  inline static uint64_t time() {

    uint32_t current = ARM_DWT_CYCCNT;

  if (current < s_lastReading) {
    __disable_irq();
      auto val = s_overflowCount + increment;
      s_overflowCount = val;
      s_lastReading = current;
    __enable_irq();
  } 
  else
    s_lastReading = current;

  return s_overflowCount + current - s_calibration;
}   

  inline void     restart()  { startTime = time();            }
  inline uint64_t elapsed()  { return time() - startTime;     }
  inline double   mS()       { return elapsed() * s_MillecondsPerTick; }
  inline double   uS()       { return elapsed() * s_MicrosecondsPerTick; }

  inline void     restartConnectTiming() { s_connectTime = time();                           }
  inline double   getConnectTime()       { return (time() - s_connectTime) * s_SecondsPerTick; }

  inline static double upTime() { return CTimer::time() * s_SecondsPerTick; }
  
  inline static double      getSecondsPerTick() { return      s_SecondsPerTick; }
  inline static double  geMillisecondsPerTick() { return   s_MillecondsPerTick; }
  inline static double getMicrosecondsPerTick() { return s_MicrosecondsPerTick; }

  static void     gpt1Handler();
  
private:
  void initGPT1();
  void callibrate();
};
