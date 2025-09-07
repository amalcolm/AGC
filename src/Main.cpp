#include "Setup.h"
#include "Hardware.h"
#include "CTimer.h"
#include "CHead.h"
#include "CUSB.h"

  const float uS_TickSpeed = 10000;  // 10ms



void setup() {
  Hardware::init();

  Head.setSequence({
    CHead::ALL_OFF,
    CHead::RED1,
    CHead::RED1 | CHead::IR1,
    CHead::IR1,
  });
}


void loop() {

  while (Timer.uS() < uS_TickSpeed);
  Timer.restart();

  Head.setNextState();

  Hardware::tick();

  USB.output_buffer();
}
