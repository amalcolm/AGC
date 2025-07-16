#include "CAutoPot.h"

CGainPot::CGainPot(int csPin, int sensorPin, int samples)
  : CAutoPot(csPin, sensorPin, samples) {}

void CGainPot::update() {
  _readSensor();

  bool invalidHigh = _lastSensorValue >= 924;
  bool invalidLow  = _lastSensorValue <= 100;

  bool boostSignal = (_lastSensorValue >= 412 && _lastSensorValue <= 812);

  if (invalidHigh || invalidLow) _offsetLevel(-1);
  else
  if (boostSignal              ) _offsetLevel(+1);

}
