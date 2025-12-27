#include "Setup.h"
#include "Hardware.h"
#include "CMasterTimer.h"
#include "CHead.h"
#include "CUSB.h"
#include "Config.h"

void setup() {
  activityLED.set();

  A2D.setCallback(_Callback);

  Hardware::begin();


  Head.setSequence( {
//    Head.ALL_OFF,
      Head.RED8,
      Head.IR7,
//    Head.RED1 | Head.IR1,            // note; use OR ( | ) to combine states
});


  Ready = true;
  activityLED.clear();
  
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
