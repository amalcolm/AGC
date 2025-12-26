#pragma once
#include <cstdint>
#include "Arduino.h"
#include "Helpers.h"
class CTimerBase {
  
protected:
  static inline uint32_t s_instanceCount = 0;
   
  static double s_SecondsPerTick;
  static double s_MillisecondsPerTick;
  static double s_MicrosecondsPerTick;

  static uint64_t s_TicksPerSecond;
  static uint64_t s_TicksPerMillisecond;
  static uint64_t s_TicksPerMicrosecond;



public:
  CTimerBase(void(* isrHandler)());


  inline uint32_t getInstanceCount() const { return s_instanceCount; }
  
  inline static double        getSecondsPerTick() { return      s_SecondsPerTick; }
  inline static double   getMillisecondsPerTick() { return s_MillisecondsPerTick; }
  inline static double   getMicrosecondsPerTick() { return s_MicrosecondsPerTick; }

  inline static uint64_t getTicksPerSecond()      { return s_TicksPerSecond;      }
  inline static uint64_t getTicksPerMillisecond() { return s_TicksPerMillisecond; }
  inline static uint64_t getTicksPerMicrosecond() { return s_TicksPerMicrosecond; }


  void initGPT1(void(* isrHandler)());  // forward to CTimer.cpp
  
private:
  void initGPT1();
};
