#pragma once
#include <cstdint>
#include "Arduino.h"
#include "Helpers.h"
class CTimer {
  
private:
   
  uint64_t startTime;
  static uint64_t s_calibration;
  static double s_SecondsPerTick;
  static double s_MillecondsPerTick;
  static double s_MicrosecondsPerTick;

  static volatile uint64_t s_connectTime;


public:
  CTimer();

  inline static uint64_t time() {
    static volatile  uint32_t s_lastReading = 0;
    static volatile  uint64_t s_offset      = 0;
    static constexpr uint64_t s_increment   = 0x1ULL << 32;

  uint32_t current = ARM_DWT_CYCCNT;
    if (current >= s_lastReading) {
      s_lastReading = current;
      return s_offset + current - s_calibration;
    }
    // Potential overflow detected - enter critical section
__disable_irq();
    current = ARM_DWT_CYCCNT;  // Re-read to handle any timing edge cases
    if (current < s_lastReading) {
      uint64_t temp = s_offset;  // Read
      temp += s_increment;       // Modify
      s_offset = temp;           // Write    
    }
    s_lastReading = current;
    TeleCount[60]++;
    uint64_t result = s_offset + current - s_calibration;
__enable_irq();

    return result;
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
