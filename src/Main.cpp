#include "Setup.h"
#include "Hardware.h"
#include "CMasterTimer.h"
#include "CHead.h"
#include "CUSB.h"
#include "Config.h"


void setup() {
  
  activityLED.set();
  Hardware::begin();

  A2D.setCallback(_Callback);

  
  Head.setSequence( {
//  Head.RED8, Head.IR8             // States defined in CHead.h, alse includes ALL_ON / ALL_OFF
//  zTest.FullTest,                // Can use predefined sequences from ZTests.h
//  Head.RED1 | Head.IR1,           // use OR ( | ) to combine LEDs
    
    Head.RED8, 
});


  Ready = true;
  activityLED.clear();

}


void loop() {
  
  Head.setNextState();              // Set the LEDs for the next state

  USB.update();                     // Output previous block, and give time for system to settle

  Head.waitForReady();              // Wait until Head is ready before starting A2D read

  while (Hardware::canUpdate())     // Loop until state duration has elapsed
    Hardware::update();              //   Update hardware components


  CTelemetry::logAll();             // Log all counter telemetry

  activityLED.toggle();             // Indicate activity on LED
  
}
