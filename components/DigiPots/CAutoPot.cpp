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


uint16_t CAutoPot::readSensor() {  if (_sensorPin < 0) return 0; // No sensor pin defined
  int32_t totalValue = 0;
  for (int i = 0; i < _samplesToAverage; i++)
    totalValue += analogRead(_sensorPin);

  _lastSensorValue = totalValue / _samplesToAverage;

  _runningAverage.add(_lastSensorValue);

  return static_cast<uint16_t>(_lastSensorValue);
}

void CAutoPot::_offsetLevel(int offset) {
  _setLevel(_currentLevel + offset);
}

void CAutoPot::_setLevel(int newLevel) {
  std::clamp(newLevel, 1, 254); 

  _currentLevel = newLevel;

  _writeToPot(_currentLevel);
}

void CAutoPot::_writeToPot(uint8_t value)
{
  static const SPISettings settings{4'800'000, MSBFIRST, SPI_MODE0};

  // Find existing entry for this CS pin (by reference)
  auto it = std::find_if(s_currentValues.begin(), s_currentValues.end(),
                        [this](const auto& entry) { return entry.first == _csPin; });

  if (it != s_currentValues.end())
  {
    if (it->second == value) return; // No change — avoid redundant SPI write
    it->second = value;
  } else {
      s_currentValues.emplace_back(_csPin, value);       // Insert new entry
  }

  SPI.beginTransaction(settings);
  {
      digitalWrite(_csPin, LOW);
      delayMicroseconds(5);

      SPI.transfer(0x00);  // Address for wiper
      SPI.transfer(value);

      digitalWrite(_csPin, HIGH);
      delayMicroseconds(5);
  }
  SPI.endTransaction();


}
