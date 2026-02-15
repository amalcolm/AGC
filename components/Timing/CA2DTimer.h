#pragma once
#include "C32bitTimer.h"
#include "CTimerBase.h"
class CA2DTimer : public C32bitTimer {
  public:
    CA2DTimer();

//    void reset(uint32_t start, uint32_t period);

    void sync() const override;



    inline void setDataReady(uint32_t tick) 
    {
      m_dataReadyPeriod = tick - m_dataReadyTick;  // uint32_t arithmetic handles wraparound correctly

      m_dataReadyTick = tick;
      m_dataReadyNextTick = tick + _period;
    }

    inline uint32_t getLastDataReadyTick()   const { return m_dataReadyTick;   }
    inline uint32_t getLastDataReadyPeriod() const { return m_dataReadyPeriod; }



  private:
    uint32_t m_dataReadyTick = 0;
    uint32_t m_dataReadyPeriod = 0;
    uint32_t m_dataReadyNextTick = 0;

};