#include "CAutoPot.h"

CGainPot::CGainPot(int csPin, int sensorPin, int samples, int windowSize)
  : CDigiPot(csPin, sensorPin, samples) {
   _lowThreshold = SENSOR_MIDPOINT - windowSize;
  _highThreshold = SENSOR_MIDPOINT + windowSize;
  }

void CGainPot::update() {
  
  readSensor();
  
  bool boostSignal = (_lastSensorValue >= _lowThreshold && _lastSensorValue <= _highThreshold);

  if (zone != Zone::inZone) _offsetLevel(-1);
  else
  if (boostSignal         ) _offsetLevel(+1);

}
