#include "CTimer.h"
#include "../teensy_compat.h"

extern "C" void irq_gpt1(void);


CTimer::CTimer() : CTimerBase(irq_gpt1) {

  if (s_instanceCount == 1)
    callibrate();
  
  restart();
}


void CTimer::callibrate() {
  s_calibration = 0;      // ensure no calibration offset
  restart();
  s_calibration = elapsed(); // ie. time taken to call the time() function, which is subtracted from all future readings
}

// Teensy 4.x hardware timer base (GPT1)
extern "C" void irq_gpt1(void) {
    GPT1_SR = GPT_SR_OF1 | GPT_SR_OC1;  // clear both flags
    CTimer::timeAbsolute();             // Handle ARM_DWT_CYCCNT overflows if timer not read frequently
}

