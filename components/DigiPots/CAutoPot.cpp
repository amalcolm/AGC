#include "CAutoPot.h"
#include "Arduino.h"
#include "SPI.h"
#include "Hardware.h"
#include "Setup.h"

CAutoPot::CAutoPot(int csPin, int sensorPin, int samplesToAverage) {
  _csPin = csPin;
  _sensorPin = sensorPin;
  _samplesToAverage = samplesToAverage > 0 ? samplesToAverage : 1;
}

// The virtual destructor definition is required.
CAutoPot::~CAutoPot() {}

void CAutoPot::begin(int initialLevel) {
  pinMode(_csPin, OUTPUT);
  digitalWrite(_csPin, HIGH);
  reset(initialLevel);
}

void    CAutoPot::reset(int level) { _setLevel(level);        }
void    CAutoPot::invert()         { _inverted = !_inverted;  }
int     CAutoPot::getLevel()       { return _currentLevel;    }
int     CAutoPot::getSensorValue() { return _lastSensorValue; }


uint32_t CAutoPot::_readSensor() {
  int32_t totalValue = 0;
  for (int i = 0; i < _samplesToAverage; i++) {
    totalValue += analogRead(_sensorPin);
  }

  _lastSensorValue = totalValue / _samplesToAverage;

  _runningAverage.Add(_lastSensorValue);

  return static_cast<uint32_t>(_lastSensorValue);
}

void CAutoPot::_offsetLevel(int offset) {
  _setLevel(_currentLevel + offset);
}

void CAutoPot::_setLevel(int newLevel) {
//  int previousLevel = _currentLevel;

if (newLevel < 1) newLevel = 1;
if (newLevel > 254) newLevel = 254; 

  

  _currentLevel = newLevel;

//  if (previousLevel != _currentLevel) {
    _writeToPot(_currentLevel);
//  }
}


void CAutoPot::_writeToPot(int value) {
  static SPISettings settings(4000000, MSBFIRST, SPI_MODE0);

  SPI.beginTransaction(settings);
  {
    digitalWrite(_csPin, LOW);
    delayMicroseconds(5);

    SPI.transfer(0x00); // Address for wiper
    
    int val = value;

    SPI.transfer(val);
    digitalWrite(_csPin, HIGH);
    delayMicroseconds(5);
  } 
  SPI.endTransaction();
}
