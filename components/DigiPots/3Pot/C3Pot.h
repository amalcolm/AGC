#pragma once
#include "CAutoPot.h"

class C3Pot : public CDigiPot {

  public:
    enum class Phase { INIT = 0, ZOOM = 1, NORMAL = 2, BACKOFF = 3, placeholder = 255} phase = Phase::INIT;
    static constexpr int HISTORY_SIZE = 4;
    static constexpr int GAP_NORMAL   = 2;

    C3Pot(int csPinTop, int csPinBot, int csPinMid, int sensorPin);

    CDigiPot  top;
    CDigiPot  bot;
    CDigiPot& mid; // also the CAutoPot instance

    bool inZone = false;

    bool lockPhase = false; 

    struct State {
      Phase       phase = Phase::placeholder;
      uint16_t   sensor = 0;
      int      topLevel = 0;
      int      botLevel = 0;
      int      midLevel = 0;
    } state{};

    State history[HISTORY_SIZE]{}; // history[0] = newest
    
    void begin(int initialLevel = 128) {
      top.invert();
      bot.invert();
      mid.invertSensor();

      top.begin(256);
      bot.begin(  0);
      mid.begin(initialLevel);
    }

    void update();

    void updateHILO(); 
    void fineTuning();

};
