#pragma once
#include "CAutoPot.h"

class COpAmp : public CAutoPot {
public:
  COpAmp(int csPinOffset, int csPinGain, int sensorPin);
  COpAmp& operator=(const COpAmp&) = default;

  COffsetPot offsetPot;
  CGainPot   gainPot;

  void begin();
  void update() override;

  void set();

  void filterSensor(int numSamples, double t);
  double _lastV = -1.0;
};