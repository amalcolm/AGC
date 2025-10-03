#include "Setup.h"
#include "Hardware.h"
#include "CTimer.h"
#include "CHead.h"
#include "CUSB.h"




void setup() {
  Hardware::init();

  Head.setSequence({
//    CHead::ALL_OFF,
    CHead::RED1,
//    CHead::RED1 | CHead::IR1,
//z    CHead::IR1,
  });
}

void trySendBlock() {
 CA2D::BlockType* toSend = nullptr;

  noInterrupts();
  if (A2D.isBlockReadyToSend) {
      toSend = A2D.getBlockToSend(); // no cast needed
      A2D.isBlockReadyToSend = false; // we’ve claimed it
  }
  interrupts();

  if (toSend == nullptr) return;
  toSend->debugSerial();
  A2D.releaseBlockToSend();
}



void loop() {
  LED.activity.toggle();

  while (Timer.uS() < TickSpeed_uS);
  Timer.restart();

  Head.setNextState();

  Hardware::tick();

  trySendBlock();
//  USB.output_buffer();
}
