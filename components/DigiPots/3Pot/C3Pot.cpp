#include "C3Pot.h"
#include "CTelemetry.h"
#include "C32bitTimer.h"
#include "CUSB.h"
#include "CBuffer.h"
#include <tuple>
C3Pot::C3Pot(int csPinTop, int csPinBot, int csPinMid, int sensorPin) 
      : CDigiPot(csPinMid, sensorPin, 2)  // pas CSmid to CAutoPot constructor
      , top(csPinTop), bot(csPinBot)
      , mid(static_cast<CDigiPot&>(*this)) {

  for (int i = 0; i < HISTORY_SIZE; ++i)
    history[i] = state;
}

C32bitTimer ti_Output = C32bitTimer::From_Hz(100).setPeriodic(true); // 100 Hz output timer for telemetry and debugging
using DebugPair = std::pair<C3Pot::State, C3Pot::State>; // current state and previous state for debugging
CBufferType<DebugPair> dbgBuffer(32); // Circular buffer to hold the history of states

void outputState(DebugPair* pPair) {
  static const char* phaseNames[] = {"INIT         ", "ZOOM     ", "NORMAL  ", "BACKOFF", "placeholder"};

  USB.printf("Phase: %s->%s, Sensor: %4d->%4d, Top: %3d->%3d,  Bot: %3d->%3d\r\n", 
      phaseNames[static_cast<int>(pPair->first.phase)], phaseNames[static_cast<int>(pPair->second.phase)],
      pPair->first.sensor, pPair->second.sensor,
      pPair->first.topLevel, pPair->second.topLevel,
      pPair->first.botLevel, pPair->second.botLevel);
}

C3Pot::Phase lastPhase = C3Pot::Phase::placeholder;

void C3Pot::update() {

  for (int i = HISTORY_SIZE - 1; i > 0; --i) history[i] = history[i - 1];
  history[0] = state;
  
  readSensor();  zone = _checkZone();

  updateHILO();


  if (phase == Phase::NORMAL) {
//    readSensor();  zone = _checkZone();
 
      fineTuning();
  }

  state.phase    = phase;
  state.sensor   = lastSensorValue();
  state.topLevel = top.getLevel();
  state.botLevel = bot.getLevel();
  state.midLevel = mid.getLevel();

  if (lastPhase != phase) {
    lastPhase = phase;

    if (dbgBuffer.isFull()) {
      ti_Output.wait();

      outputState(dbgBuffer.read());
    }
    dbgBuffer.write({state, history[0]});
  }



  if (ti_Output.passed()) {

    if (dbgBuffer.isEmpty() == false)
      outputState(dbgBuffer.read());

  }
};


