#include "Setup.h"
#include "Hardware.h"
#include "CTimer.h"
#include "CHead.h"
#include "CUSB.h"

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
  const float uS_delay = 10000;  // 10ms

  while (Timer.uS() < uS_delay);
  Timer.restart();

  Head.setNextState();

  USB.output_buffer();
}
