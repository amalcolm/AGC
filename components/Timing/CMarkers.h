#pragma once
#include "CTimer.h"

class CMarker32 {
private:
  uint32_t _period = 0;
  bool _isPeriodic = false;
  mutable uint32_t _nextMarker = 0;
  mutable uint32_t _lastMarker = 0;

  CMarker32();

public:
  static CMarker32 From_uS(double uS);
  static CMarker32 From_mS(double mS);
  static CMarker32 From_S (double  S);
  static CMarker32 From_Hz(double Hz);

  inline CMarker32& setPeriodic(bool isPeriodic) { _isPeriodic = isPeriodic; return *this; }
  inline bool       getPeriodic() const { return _isPeriodic; }

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
    if (diff < 0) return false;
    _lastMarker = _nextMarker;

    if (_isPeriodic) _nextMarker += _period;
    return true;
  }

  inline bool waiting() const {
    uint32_t now = ARM_DWT_CYCCNT;  if (_period == 0) return false;
    int32_t diff = static_cast<int32_t>(now - _nextMarker);
    if (diff < 0) return true;
    _lastMarker = _nextMarker;

    if (_isPeriodic) _nextMarker += _period;
    return false;
  }

  inline void wait() const {    if (_period == 0) return;

    while (true) {
      uint32_t now = ARM_DWT_CYCCNT;
      int32_t diff = static_cast<int32_t>(now - _nextMarker);
      if (diff >= 0) break;
      yield();
    }

    _lastMarker = _nextMarker;
    if (_isPeriodic) _nextMarker += _period;
  }

  void A2DWait() const;

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

  inline uint32_t getRemainingTicks() const { return (_nextMarker - ARM_DWT_CYCCNT); }
         double   getRemaining_uS()   const;
         double   getRemaining_mS()   const;
         double   getRemaining_S ()   const;

};
