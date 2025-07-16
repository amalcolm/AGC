
class Timer {
private:
  uint32_t startTick;
  uint32_t calibration;
  float ticksPerMS;
  float ticksPerUS;

public:
  Timer() {
    ARM_DWT_CTRL |= ARM_DWT_CTRL_CYCCNTENA;  // enable cycle counter
    
    ticksPerMS = F_CPU / 1000.0f;
    ticksPerUS = F_CPU / 1000000.0f;

    restart();
    calibration = ticks_raw();
  }

  inline void     restart()     { startTick = ARM_DWT_CYCCNT; }
  inline uint32_t ticks() const { return ticks_raw() - calibration; }
  inline float    mS() const    { return ticks() / ticksPerMS; }
  inline float    uS() const    { return ticks() / ticksPerUS; }

private:
  inline uint32_t ticks_raw() const { return ARM_DWT_CYCCNT - startTick; }
};
