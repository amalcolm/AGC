#pragma once
#include "RunningAverage.h"


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

  RunningAverage& getRunningAverage() { return _runningAverage; }


protected:
  int  _readSensor();
  void _offsetLevel(int offset);

  int _csPin;
  int _sensorPin;
  int _samplesToAverage;
  int _currentLevel = 127;
  int _lastSensorValue = 0;

  bool _inverted = false;
  RunningAverage _runningAverage;

private:
  void _setLevel(int newLevel);
  void _writeToPot(int value);
};

// =================================================================
class COffsetPot : public CAutoPot {
public:
  COffsetPot(int csPin, int sensorPin, int samples, int lowThreshold, int highThreshold);
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