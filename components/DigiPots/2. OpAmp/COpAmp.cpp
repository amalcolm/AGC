#include "COpAmp.h"
#include "Arduino.h"
#include "CMasterTimer.h"

constexpr int OFFSET_WINDOW_SIZE = 400; // 280 normal
constexpr int   GAIN_WINDOW_SIZE = 400; // 100 normal

constexpr int SAMPLES_TO_AVERAGE = 52;

COpAmp::COpAmp(int csPinOffset, int csPinGain, int sensorPin)
: CAutoPot(-1, sensorPin, SAMPLES_TO_AVERAGE)
, offsetPot(csPinOffset, sensorPin, 1, OFFSET_WINDOW_SIZE)
, gainPot  (csPinGain  , sensorPin, 1,   GAIN_WINDOW_SIZE) {}

void COpAmp::begin() {
  gainPot  .invert();

  offsetPot.begin(128); 
    gainPot.begin(  0);
}

void COpAmp::set() {
  offsetPot.writeCurrentToPot();
    gainPot.writeCurrentToPot();
}


void COpAmp::update() {

  offsetPot.update();   

  if (offsetPot.inZone)
    gainPot.update();

  if (Timer.sampleReady) return;

  if (gainPot.inZone) {

    if (Timer.getStateTime() > 0.001) {
      readSensor(); 
      _updateZone();

      Timer.sampleReady = true;
    }
  }
  else
  {
    inZone = false;
    if (analogRead(getSensorPin()) > CAutoPot::SENSOR_MIDPOINT)
      zone = Zone::High;
    else
      zone = Zone::Low;
  }
}