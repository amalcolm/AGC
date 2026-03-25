#pragma once
#include "CRunningAverage.h"
#include <utility>
#include <deque>

class CAutoPot {
public:
  bool inZone = false;
  enum class Zone { Low = -1, inZone = 0, High = +1, Placeholder = 255} ;
  Zone zone = Zone::Placeholder;
  static constexpr int SENSOR_MIDPOINT = 512;
  static constexpr int POT_MIN = 0;
  static constexpr int POT_MAX = 255;
  static constexpr int POT_MIDPOINT = (POT_MAX + POT_MIN) / 2;

  CAutoPot(int csPin, int sensorPin, int samplesToAverage);
  virtual ~CAutoPot(); // Needed to call destructor of subclass

  void begin(int initialLevel = 128);
  void reset(int level);
  void invert();
  void invertSensor();
  virtual void update() = 0; // must be overridden

  uint16_t   readSensor();

  inline int getLevel()        const { return _currentLevel;    }
  inline int lastSensorValue() const { return _lastSensorValue; }
  inline int getSensorPin()    const { return _sensorPin;       }

  inline void writeCurrentToPot() { _writeToPot(_currentLevel); }
  
  CRunningAverageMinMax<uint16_t>& getRunningAverage() { return _runningAverage; }


protected:

  inline void _setLevel(int newLevel) {
    _currentLevel = std::clamp(newLevel, POT_MIN, POT_MAX); 
    _writeToPot(_currentLevel);
  }

  inline void _offsetLevel(int offset) { _setLevel(_currentLevel + offset); }


  Zone   _checkZone();


  int _csPin; 
  int _sensorPin;
  int _samplesToAverage;
  int _currentLevel = -1;
  int _lastSensorValue = 0;

  bool _inverted = false;
  bool _invertedSensor = false;
  CRunningAverageMinMax<uint16_t> _runningAverage;

private:
  void _writeToPot(int value);
};

// =================================================================
class CDigiPot : public CAutoPot {
public:
  CDigiPot(int csPin) : CAutoPot(csPin, -1, 1) {}
  CDigiPot(int csPin, int sensorPin, int samplesToAverage) : CAutoPot(csPin, sensorPin, samplesToAverage) {}
  CDigiPot& operator=(const CDigiPot&) = default;
  void update() override { } // no update needed but override is
                             // required to be non-abstract
  void setLevel(int level) { _setLevel(level); }
  void offsetLevel(int offset) { _offsetLevel(offset); }
  uint16_t readAverage(int samples = -1) { if (samples == -1) return readSensor(); 
    int oldSamplesToAverage = _samplesToAverage;
    _samplesToAverage = samples;
    uint16_t average = readSensor();
    _samplesToAverage = oldSamplesToAverage;
    return average;
  }
};
// =================================================================

class COffsetPot : public CAutoPot {
public:
  COffsetPot(int csPin, int sensorPin, int samples, int windowSize);
  COffsetPot& operator=(const COffsetPot&) = default;
  void update() override;

private:
  int _lowThreshold;
  int _highThreshold;
};

// =================================================================
class CGainPot : public CAutoPot {
public:
  CGainPot(int csPin, int sensorPin, int samples);
  CGainPot& operator=(const CGainPot&) = default;
  void update() override;
};

#include "3Pot/C3Pot.h"

static_assert(std::is_copy_constructible_v<COffsetPot>);
static_assert(std::is_copy_constructible_v<CGainPot  >);
static_assert(std::is_copy_assignable_v<COffsetPot>);
static_assert(std::is_copy_assignable_v<CGainPot  >);