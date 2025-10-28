#include "Setup.h"
#include "Hardware.h"
#include "CTimer.h"
#include "CHead.h"
#include "CUSB.h"



void ProccessA2D(volatile BlockType* block) { if (block->data == NULL) return;

  auto& [state, offsetPot1, offsetPot2, gainPot] = getPerStateHW();

  DataType data = A2D.getData();

  auto avg1 = offsetPot1.getRunningAverage().GetAverage() - 900;
  auto avg2 = offsetPot2.getRunningAverage().GetAverage() - 512;

  // --- Serial Debugging Output ---
  Serial.print("\r\n A2D:");      Serial.print(data.channels[0]/6000);
  Serial.print(  "\t Sensor1:");  Serial.print(offsetPot1.getSensorValue()-0*avg1);
  Serial.print(  "\t Sensor2:");  Serial.print(offsetPot2.getSensorValue()-0*avg2);
  Serial.print(  "\t offset1:");  Serial.print(offsetPot1.getLevel());
  Serial.print(  "\t offset2:");  Serial.print(offsetPot2.getLevel());
  Serial.print(  "\t Gain:");     Serial.print(   gainPot.getLevel());
  Serial.print(  "\t Min:");      Serial.print(offsetPot2.getRunningAverage().GetMin() - 0*avg2);
  Serial.print(  "\t Max:");      Serial.print(offsetPot2.getRunningAverage().GetMax() - 0*avg2);

}








void setup() {
  Hardware::init();
  A2D.setCallback(ProccessA2D);

  Head.setSequence({
//    CHead::ALL_OFF,
      CHead::RED1,
//    CHead::RED1 | CHead::IR1,
//    CHead::IR1,
  });
}




void loop() {
  Head.setNextState();

  while (Timer.uS() < TickSpeed_uS) A2D.poll();
  Timer.restart();

  Hardware::tick();

  LED.activity.toggle();
}
