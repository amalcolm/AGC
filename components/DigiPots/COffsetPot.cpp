#include "CAutoPot.h"
#include "Setup.h"

COffsetPot::COffsetPot(int csPin, int sensorPin, int samples, int windowSize)
  : CAutoPot(csPin, sensorPin, samples),
   _lowThreshold(490 - windowSize), 
  _highThreshold(490 + windowSize) {

    _runningAverage.reset(160);
}

void COffsetPot::update() {
  auto val =  _readSensor();

  //  val = _runningAverage.GetAverage();

  inZone = (val > _lowThreshold && val < _highThreshold);

  if (val > _highThreshold) _offsetLevel(+1); 
  else
  if (val <  _lowThreshold) _offsetLevel(-1);
}





