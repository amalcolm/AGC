#pragma once
#include "C32bitTimer.h"
#include "CTimerBase.h"
class CA2DTimer : public C32bitTimer {
public:
  CA2DTimer();

  void reset(uint32_t start, uint32_t period);

  void A2DWait() const;


};