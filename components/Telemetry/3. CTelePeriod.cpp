#include "3. CTelePeriod.h"
#include "Arduino.h"
#include "CTimer.h"

CTelePeriod::CTelePeriod(TeleGroup group, uint16_t id) : CTelemetry(group, SUBGROUP, id) {
  
  if (ID == 0xFFFF) ID = instanceCounter++;

  _register(this);
}


float CTelePeriod::getValue() {
  float val = static_cast<float>(_maxTick * CTimer::getMicrosecondsPerTick());
  
  _maxTick = 0;  // reset so next call skips first measurement again
  return val;
}
