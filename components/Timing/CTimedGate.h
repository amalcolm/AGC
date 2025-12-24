#pragma once
#include <cstdint>

class CTimedGate {
  public:
    CTimedGate(double period);

  public:
    virtual bool pass();
    virtual bool block();
    
  private:
    uint64_t _period;
    uint64_t nextTime;
};

class CA2DTimedGate : public CTimedGate {
  private:
    bool passed = false;

  public:
    CA2DTimedGate(double period);

    bool pass() override;
    bool block() override;
};