#include "Setup.h"
#include "Hardware.h"
#include "CMasterTimer.h"
#include "CHead.h"
#include "CUSB.h"
#include "Config.h"
#include "Helpers.h"
#include "CTelemetry.h"

BlockType dbgBlock1;
BlockType dbgBlock2;

void _setup() {
  activityLED.set();

  Hardware::begin(); 

  Ready = true;

  dbgBlock1.clear();
  dbgBlock2.clear();
  
  activityLED.clear();
}


uint32_t count = 0;
void _loop() {
  static BlockType* dbgBlock = &dbgBlock1;

  auto [state, TIA, offsetPot2, gainPot, _] = getHWforState(Head.RED1);

  USB.update();
//  USB.waitForHandshake();

  Timer.state.reset();

  TIA.findSignal();




//  USB.buffer(dbgBlock);

  dbgBlock = (dbgBlock == &dbgBlock1) ? &dbgBlock2 : &dbgBlock1;
  
  dbgBlock->clear();


//  CTelemetry::logAll();
  

  USB.printf("\n");
  delay(500);
 activityLED.toggle();             // Indicate activity on LED
}
