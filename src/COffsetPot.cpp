#include "CAutoPot.h"


COffsetPot::COffsetPot(int csPin, int sensorPin, int samples, int lowThreshold, int highThreshold)
  : CAutoPot(csPin, sensorPin, samples), _lowThreshold(lowThreshold), _highThreshold(highThreshold) {
}

void COffsetPot::update() {
  _readSensor();

  inZone = (_lastSensorValue > _lowThreshold && _lastSensorValue < _highThreshold);

  if (_lastSensorValue > _highThreshold) _offsetLevel( 1); 
  else
  if (_lastSensorValue <  _lowThreshold) _offsetLevel(-1);
}
