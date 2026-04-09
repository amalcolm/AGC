#include "COpAmp.h"
#include "Arduino.h"
#include "Hardware.h"

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

  if (HW && HW->offsetsChanged) {
     HW->offsetsChanged = false;
    _lastV = static_cast<float>(analogRead(getSensorPin()));
  }


  if (gainPot.inZone) {

    if (Timer.getStateTime() > 0.001f) {
      filterSensor(SAMPLES_TO_AVERAGE, 0.002f);
      Timer.sampleReady = true;
    } else {
      filterSensor(1, 0.01f);
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

void COpAmp::filterSensor(int numSamples, float t) {
  float tInv = 1.0f - t;
  int sensor = getSensorPin();

  float v = _lastV;

  for (int i = 0; i < numSamples; ++i)
    v = t * static_cast<float>(analogRead(sensor)) + tInv * v;

  _lastV = v;
  _lastSensorValue = static_cast<int>(v);

  _updateZone();
}