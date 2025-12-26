#include "Setup.h"
#include "Hardware.h"
#include "CMasterTimer.h"
#include "CHead.h"
#include "CUSB.h"

bool TESTMODE = false;  // if true, uses polled A2D mode and _Callback by default

constexpr int LoopPeriod_mS = 20;  // 20ms

void _Callback(BlockType* block);  // forward declaration of _Callback.cpp

void setup() {
  activityLED.set();

  A2D.setCallback(_Callback);

  Hardware::begin();

  Head.setSettleTime(0.5); // 500 microseconds settle time

  Head.setSequence( {
//    Head.ALL_OFF,
      Head.RED8,
      Head.IR7,
//    Head.RED1 | Head.IR1,            // note; use OR ( | ) to combine states
});


  Ready = true;
  activityLED.clear();
  Timer.setPeriodMS(LoopPeriod_mS);
}


void loop() {

  Head.setNextState();    // Set the LEDs for the next state

  getPerStateHW().set();  // apply HW settings for new state

  USB.update();           // output previous block, and give time for system to settle

  Head.waitForReady();    // wait until Head is ready before starting A2D read

  while (Timer.isProcessingState()) Hardware::update();  // update hardware until period elapses
  
  CTelemetry::logAll();  // log all counter telemetry

  activityLED.toggle();
}
