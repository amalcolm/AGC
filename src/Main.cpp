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
//  Head.ALL_OFF,
//  Head.RED4, Head.IR4,
    Head.RED1, Head.RED2, Head.RED3, Head.RED4, Head.RED5, Head.RED6, Head.RED7, Head.RED8, Head. IR1, Head. IR2, Head. IR3, Head. IR4, Head. IR5, Head. IR6, Head. IR7, Head. IR8,
    Head.RED1 | Head.IR1, Head.RED2 | Head.IR2, Head.RED3 | Head.IR3, Head.RED4 | Head.IR4, Head.RED5 | Head.IR5, Head.RED6 | Head.IR6, Head.RED7 | Head.IR7, Head.RED8 | Head.IR8,
    Head.RED1 | Head.RED2 | Head.IR1 | Head.IR2, 
    Head.RED3 | Head.RED4 | Head.IR3 | Head.IR4, 
    Head.RED5 | Head.RED6 | Head.IR5 | Head.IR6,
    Head.RED7 | Head.RED8 | Head.IR7 | Head.IR8,
    
    Head.ALL_OFF, Head.ALL_ON,
//  Head.RED1 | Head.IR1,            // note; use OR ( | ) to combine LEDs
});


  Ready = true;
  activityLED.clear();
  
}


void loop() {

  Head.setNextState();    // Set the LEDs for the next state

  getPerStateHW().set();  // apply HW settings for new state

  USB.update();           // output previous block, and give time for system to settle

  Head.waitForReady();    // wait until Head is ready before starting A2D read

  while (Timer.state.waiting()) Hardware::update();  // update hardware until period elapses
  
  CTelemetry::logAll();  // log all counter telemetry

  activityLED.toggle();
}
