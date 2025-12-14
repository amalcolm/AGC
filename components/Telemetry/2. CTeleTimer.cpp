#include "2. CTeleTimer.h"
#include "CTimer.h"
#include "Setup.h"

CTeleTimer::CTeleTimer(TeleGroup group, uint16_t id) : CTelemetry(group, SUBGROUP, id) {
  if (id == 0xFFFF) 
    ID = instanceCounter++;

  _register(this);
}

float CTeleTimer::getValue()  {
  uint32_t retVal = _maxDuration * CTimer::getMicrosecondsPerTick();
  _maxDuration = 0;
  return retVal;
}

void CTeleTimer::stop() {
    uint32_t duration = ARM_DWT_CYCCNT - _start;

    if (first)
      USB.printf("Timer %p stop() value = %8.3f\n", this, getValue());

    if (duration > _maxDuration)
      _maxDuration = duration;

    first = false;
  }
