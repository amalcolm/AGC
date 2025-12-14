#include "2. CTeleTimer.h"
#include "CTimer.h"
#include "Setup.h"

CTeleTimer::CTeleTimer(TeleGroup group, uint16_t id) : CTelemetry(group, SUBGROUP, id) {
  if (id == 0xFFFF) 
    ID = instanceCounter++;
    
  USB.printf("CTeleTimer constructed %p\n", this);

  _register(this);
}

CTeleTimer::~CTeleTimer() {
  USB.printf("CTeleTimer destructed %p\n", this);
} 

float CTeleTimer::getValue()  {
  float retVal = static_cast<float>(_maxDuration * CTimer::getMicrosecondsPerTick());
  _maxDuration = 0;
  return retVal;
}

void CTeleTimer::stop() {
    uint32_t duration = ARM_DWT_CYCCNT - _start;

    if (duration > _maxDuration)
      _maxDuration = duration;

    if (first)
      USB.printf("Timer %p stop() value = %8.3f\n", this, getValue());

    first = false;
  }
