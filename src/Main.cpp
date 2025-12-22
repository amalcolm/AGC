#include "Setup.h"
#include "Hardware.h"
#include "CTimer.h"
#include "CHead.h"
#include "CUSB.h"

bool TESTMODE = false;  // if true, uses polled A2D mode and _Callback by default

constexpr double LoopPeriod_uS = 20000;  // 20ms

void _Callback(BlockType* block);

void setup() {
  activityLED.set();

  A2D.setCallback(_Callback);

  Hardware::begin();

  Head.setSettleTime(500); // 500 microseconds settle time

  Head.setSequence( {
//    Head.ALL_OFF,
      Head.RED8,
      Head.IR8,
//    Head.RED1 | Head.IR1,            // note; use OR ( | ) to combine states
});


  Ready = true;
  activityLED.clear();
}

CTeleTimer TT_loop{TeleGroup::TIMER, 0x07};

void loop() {
  Head.setNextState();    // Set the LEDs for the next state
  TT_loop.start();

  getPerStateHW().set();  // apply HW settings for new state

  USB.update();           // output previous block, and give time for system to settle
  TT_loop.stop();

  Head.waitForReady();    // wait until Head is ready before starting A2D read

  while (Timer.uS() < LoopPeriod_uS) Hardware::update();  // update hardware until period elapses
  Timer.restart();

  CTelemetry::logAll();  // log all counter telemetry

  activityLED.toggle();
}
