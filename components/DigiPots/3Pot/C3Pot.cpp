#include "C3Pot.h"
#include "CTelemetry.h"
#include "C32bitTimer.h"
#include "Setup.h" 
#include "CUSB.h"
#include "CBuffer.h"

C3Pot::C3Pot(int csPinTop, int csPinBot, int csPinMid, int sensorPin) 
      : CAutoPot(csPinMid, sensorPin, 2)  // pas CSmid to CAutoPot constructor
      , top(csPinTop), bot(csPinBot)
      , mid(static_cast<CAutoPot&>(*this)) {

  for (int i = 0; i < HISTORY_SIZE; ++i)
    history[i] = state;
}

C32bitTimer ti_Output = C32bitTimer::From_Hz(100).setPeriodic(true); // 100 Hz output timer for telemetry and debugging
CBufferType<C3Pot::State> dbgBuffer(32); // Circular buffer to hold the history of states

C3Pot::Phase lastPhase = C3Pot::Phase::placeholder;

void C3Pot::update() {

  for (int i = HISTORY_SIZE - 1; i > 0; --i) history[i] = history[i - 1];
  history[0] = state;
  
  readSensor();
  zone = _checkZone();

  switch (phase) 
  {
    case Phase::INIT   : doInit();    break;
    case Phase::ZOOM   : doZoom();    break;
    case Phase::NORMAL : doNormal();  break;
    case Phase::BACKOFF: doBackoff(); break;

    default: break;
  }


  state.phase    = phase;
  state.sensor   = lastSensorValue();
  state.topLevel = top.getLevel();
  state.botLevel = bot.getLevel();
  state.midLevel = mid.getLevel();

  if (lastPhase != phase) {
    lastPhase = phase;
    dbgBuffer.write(state); 
  }



  if (ti_Output.passed()) {

    if (dbgBuffer.isEmpty() == false) {
      State dbgState;
      dbgBuffer.read(dbgState);
      USB.printf("Phase: %d, Sensor: %4d, Top: %3d, Mid: %3d, Bot: %3d\r\n", 
        static_cast<int>(dbgState.phase), dbgState.sensor, dbgState.topLevel, dbgState.midLevel, dbgState.botLevel);
    }

  }
};


