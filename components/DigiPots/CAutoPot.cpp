#include "CAutoPot.h"
#include "Arduino.h"
#include "SPI.h"
#include "Hardware.h"
#include "Setup.h"

static std::array<int, 48> _potValueCache = {-2};

CAutoPot::CAutoPot(int csPin, int sensorPin, int samplesToAverage) {
  _csPin = csPin;
  _sensorPin = sensorPin;
  _samplesToAverage = samplesToAverage > 0 ? samplesToAverage : 1;

  if (_potValueCache[0] == -2)
    _potValueCache.fill(-1); // fill cache with invalid values
}

// The virtual destructor definition is required.
CAutoPot::~CAutoPot() {}

void CAutoPot::begin(int initialLevel) {
  pinMode(_csPin, OUTPUT);
  digitalWrite(_csPin, HIGH);
  reset(initialLevel);
}

void    CAutoPot::reset(int level) { _setLevel(level);                   }
void    CAutoPot::invert()         { _inverted       = !_inverted;       }
void    CAutoPot::invertSensor()   { _invertedSensor = !_invertedSensor; }

CAutoPot::Zone CAutoPot::_checkZone() {
 static constexpr int       DEADZONE = 64;

 static constexpr int  LOW_THRESHOLD =        DEADZONE;
 static constexpr int HIGH_THRESHOLD = 1023 - DEADZONE;

 Zone newZone;

 if (_lastSensorValue <  LOW_THRESHOLD) newZone = Zone::Low;
 else
 if (_lastSensorValue > HIGH_THRESHOLD) newZone = Zone::High;
 else
   newZone = Zone::inZone;

 inZone = (newZone == Zone::inZone);
 return newZone;
}

uint16_t CAutoPot::readSensor() {  if (_sensorPin < 0) return 0; // No sensor pin defined
  int totalValue = 0;
  for (int i = 0; i < _samplesToAverage; i++)
      totalValue += analogRead(_sensorPin);

  if (_invertedSensor)
      totalValue = (_samplesToAverage * 1023) - totalValue;

  _lastSensorValue = totalValue / _samplesToAverage;
  _runningAverage.add(_lastSensorValue);

  zone = _checkZone();

  return static_cast<uint16_t>(_lastSensorValue);
}

void CAutoPot::_offsetLevel(int offset) {
  _setLevel(_currentLevel + offset);
}

void CAutoPot::_setLevel(int newLevel) {

  _currentLevel = std::clamp(newLevel, 0, 255); 

  _writeToPot(_currentLevel);
}

void CAutoPot::_writeToPot(int value) {
  static const SPISettings settings{8'000'000, MSBFIRST, SPI_MODE0};

  if (value < 0 || value > 255) return;
  
//  if (_potValueCache[_csPin] == value) return; // No change — avoid redundant SPI write
//  _potValueCache[_csPin] = value; // Update cache with new value


  uint8_t potValue = _inverted ? static_cast<uint8_t>(255-value) 
                               : static_cast<uint8_t>(    value);

  SPI.beginTransaction(settings);
  {
      digitalWrite(_csPin, LOW);
      delayMicroseconds(2);

      SPI.transfer(0x00);  // Address for wiper
      SPI.transfer(potValue);

      digitalWrite(_csPin, HIGH);
      delayMicroseconds(2);
  }
  SPI.endTransaction();
}
