#include "CAutoPot.h"


COffsetPot::COffsetPot(int csPin, int sensorPin, int samples, int lowThreshold, int highThreshold)
  : CAutoPot(csPin, sensorPin, samples), _lowThreshold(lowThreshold), _highThreshold(highThreshold) {

    _runningAverage.Reset(100);
}

void COffsetPot::update() {
  auto val =  _readSensor();
 

  inZone = (val > _lowThreshold && val < _highThreshold);

  if (val > _highThreshold) _offsetLevel( 1); 
  else
  if (val <  _lowThreshold) _offsetLevel(-1);
}
