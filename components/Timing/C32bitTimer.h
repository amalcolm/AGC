#pragma once
#include "CTimerBase.h"

class C32bitTimer : public CTimerBase {
protected:
  uint32_t _period = 0;
  bool _isPeriodic = false;

  // allow const methods to update these markers
  mutable uint32_t _lastMarker = 0;  
  mutable uint32_t _nextMarker = 0;

  C32bitTimer();

public:
  static C32bitTimer From_uS(double uS);
  static C32bitTimer From_mS(double mS);
  static C32bitTimer From_S (double  S);
  static C32bitTimer From_Hz(double Hz);

  inline C32bitTimer& setPeriodic(bool isPeriodic) { _isPeriodic = isPeriodic; return *this; }
  inline bool         getPeriodic() const { return _isPeriodic; }

  inline uint32_t getLastMarker() const { return _lastMarker; }
  inline uint32_t getTicks() const { return ARM_DWT_CYCCNT - _lastMarker; }

  inline double      getSeconds() const { return getTicks() * CTimerBase::getSecondsPerTick();      }
  inline double getMilliseconds() const { return getTicks() * CTimerBase::getMillisecondsPerTick(); }
  inline double getMicroseconds() const { return getTicks() * CTimerBase::getMicrosecondsPerTick(); }

  inline double      getSeconds(uint32_t mark) const { return static_cast<int32_t>(mark - _lastMarker) * CTimerBase::getSecondsPerTick();      }
  inline double getMilliseconds(uint32_t mark) const { return static_cast<int32_t>(mark - _lastMarker) * CTimerBase::getMillisecondsPerTick(); }
  inline double getMicroseconds(uint32_t mark) const { return static_cast<int32_t>(mark - _lastMarker) * CTimerBase::getMicrosecondsPerTick(); }

  inline bool passed() const {
    uint32_t now = ARM_DWT_CYCCNT;  if (_period == 0) return true;
    int32_t diff = static_cast<int32_t>(now - _nextMarker);
    if (diff <= 0) return false;

    _lastMarker = _nextMarker;
    if (_isPeriodic) _nextMarker += _period;
    return true;
  }

  inline bool waiting() const {
    uint32_t now = ARM_DWT_CYCCNT;  if (_period == 0) return false;
    int32_t diff = static_cast<int32_t>(now - _nextMarker);
    if (diff <= 0) return true;

    _lastMarker = _nextMarker;
    if (_isPeriodic) _nextMarker += _period;
    return false;
  }

   inline void wait() const { if (_period == 0) return;

    while (static_cast<int32_t>(ARM_DWT_CYCCNT - _nextMarker) < 0)
      yield();

    _lastMarker = _nextMarker;
    if (_isPeriodic) _nextMarker += _period;
  }


  inline uint32_t reset() const {
    _lastMarker = ARM_DWT_CYCCNT;
    _nextMarker = _lastMarker + _period;
    return _lastMarker;
  }

  inline void resetAfter(uint32_t delta) const {
    _lastMarker = ARM_DWT_CYCCNT;
    _nextMarker = _lastMarker + delta;
  }

  inline void resetAt(uint32_t time) const {
    _lastMarker = ARM_DWT_CYCCNT;
    _nextMarker = time;
  }

  inline void sync(uint32_t now = ARM_DWT_CYCCNT) const {
    _nextMarker = now - ((now - _lastMarker) % _period);
    _lastMarker = _nextMarker - _period;
  }

  inline uint32_t getPeriodTicks()  const { return _period; }
  inline double   getPeriod_uS()    const { return _period * CTimerBase::getMicrosecondsPerTick(); }
  inline double   getPeriod_mS()    const { return _period * CTimerBase::getMillisecondsPerTick(); }
  inline double   getPeriod_S ()    const { return _period * CTimerBase::getSecondsPerTick();      }

  inline  int32_t getRemainingTicks() const { return _period == 0 ? 0 : static_cast<int32_t>(_nextMarker - ARM_DWT_CYCCNT); }
  inline double   getRemaining_uS()   const { return getRemainingTicks() * CTimerBase::getMicrosecondsPerTick(); }
  inline double   getRemaining_mS()   const { return getRemainingTicks() * CTimerBase::getMillisecondsPerTick(); }
  inline double   getRemaining_S()    const { return getRemainingTicks() * CTimerBase::getSecondsPerTick();      }
};
