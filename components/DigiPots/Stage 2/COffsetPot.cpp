#include "CAutoPot.h"
#include "Setup.h"

// use 490 as a centre point for the physical pot
COffsetPot::COffsetPot(int csPin, int sensorPin, int samples, int windowSize)
  : CDigiPot(csPin, sensorPin, samples),
   _lowThreshold(512 - windowSize), 
  _highThreshold(512 + windowSize) {

    _runningAverage.reset(160);
}

void COffsetPot::update() {
  auto val =  readSensor();

  //  val = _runningAverage.GetAverage();

  if (val > _highThreshold) _offsetLevel(+1); 
  else
  if (val <  _lowThreshold) _offsetLevel(-1);
}





