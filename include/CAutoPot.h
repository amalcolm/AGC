#ifndef _CAUTOPOT
#define _CAUTOPOT

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

protected:
  void _readSensor();
  void _offsetLevel(int offset);

  int _csPin;
  int _sensorPin;
  int _samplesToAverage;
  int _currentLevel = 127;
  int _lastSensorValue = 0;

  bool _inverted = false;

private:
  void _setLevel(int newLevel);
  void _writeToPot(int value);
};

// =================================================================
class COffsetPot : public CAutoPot {
public:
  COffsetPot(int csPin, int sensorPin, int samples, int lowThreshold, int highThreshold);
  void update() override;

private:
  int _lowThreshold;
  int _highThreshold;;
};

// =================================================================
class CGainPot : public CAutoPot {
public:
  CGainPot(int csPin, int sensorPin, int samples);
  void update() override;
};

#endif