#pragma once
#include "C32bitTimer.h"
#include "CTimerBase.h"
class C32bitA2DTimer : public C32bitTimer {
public:
  C32bitA2DTimer();

  void reset(uint32_t start, uint32_t period);

  void A2DWait() const;


};