#include "CAutoPot.h" // Assumes the header file is named CAutoPot.h
#include "Arduino.h"
#include "SPI.h"


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

void CAutoPot::reset(int level) {
  _setLevel(level);
}

void CAutoPot::invert() {
  _inverted = !_inverted;
}

int CAutoPot::getLevel() {
  return _currentLevel;
}

int CAutoPot::getSensorValue() {
  return _lastSensorValue;
}


void CAutoPot::_readSensor() {
  long totalValue = 0;
  for (int i = 0; i < _samplesToAverage; i++) {
    totalValue += analogRead(_sensorPin);
  }
  _lastSensorValue = totalValue / _samplesToAverage;
}

void CAutoPot::_offsetLevel(int offset) {
  _setLevel(_currentLevel + offset);
}

void CAutoPot::_setLevel(int newLevel) {
  int previousLevel = _currentLevel;
  _currentLevel = constrain(newLevel, 1, 254);
  if (previousLevel != _currentLevel) {
    _writeToPot(_currentLevel);
  }
}

void CAutoPot::_writeToPot(int value) {
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  digitalWrite(_csPin, LOW);
  SPI.transfer(0x00); // Address for wiper
  SPI.transfer(_inverted ? 255 - value : value);
  digitalWrite(_csPin, HIGH);
  SPI.endTransaction();
}
