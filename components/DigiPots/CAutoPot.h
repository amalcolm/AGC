#pragma once
#include "RunningAverage.h"
#include <utility>
#include <deque>

class CAutoPot {
public:
  bool inZone = false;

  CAutoPot(int csPin, int sensorPin, int samplesToAverage);
  virtual ~CAutoPot(); // Needed to call destructor of subclass

  void begin(int initialLevel = 127);
  void reset(int level);
  void invert();
  virtual void update() = 0; // must be overridden

  int getLevel();
  int getSensorValue();
  int getSensorPin() const { return _sensorPin; }

  void writeCurrentToPot() { _writeToPot(_currentLevel); }
  
  RunningAverageMinMax<uint16_t>& getRunningAverage() { return _runningAverage; }


protected:
  uint32_t _readSensor();
  void     _offsetLevel(int offset);
  void     _setLevel(int newLevel);

  int _csPin; 
  int _sensorPin;
  int _samplesToAverage;
  int _currentLevel = -1;
  int _lastSensorValue = 0;

  inline static std::deque<std::pair<uint8_t, uint8_t>> s_currentValues{};

  bool _inverted = false;
  RunningAverageMinMax<uint16_t> _runningAverage;

private:
  void _writeToPot(uint8_t value);
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



static_assert(std::is_copy_constructible_v<COffsetPot>);
static_assert(std::is_copy_constructible_v<CGainPot  >);
static_assert(std::is_copy_assignable_v<COffsetPot>);
static_assert(std::is_copy_assignable_v<CGainPot  >);