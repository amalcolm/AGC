#include "Setup.h"
#include "Hardware.h"
#include "CTimer.h"
#include "CHead.h"
#include "CUSB.h"

bool TESTMODE = true;  // if true, uses polled A2D mode and ProccessA2D callback by default

constexpr float LoopPeriod_uS = 20000;  // 20ms

COffsetPot    offsetPot1{ CS.offset1, SP.postGain,  50, 224, 800 };

// ProcessA2D: Callback to process A2D data blocks for debugging
void ProccessA2D(BlockType* block) {//  if (!TESTMODE || block == nullptr || block->count == 0) return;

  
  activityLED.toggle();



  int A2D_value = offsetPot1.getSensorValue();
  int Offset1   = offsetPot1.getLevel();

  Serial.print("Min:0\tMax:1024\t"); 

  Serial.print("Sens:");    Serial.print(A2D_value);  Serial.print("\t");
  Serial.print("Off1:");    Serial.print(Offset1);    Serial.print("\t");

  Serial.println(); 







return;









/*
  // if we are outputting the A2D data, skip the debug below
  A2D.outputDebugBlock = false;  if (A2D.outputDebugBlock) return; 

  // get hardware for the block's state
  //auto& [state, offsetPot1, offsetPot2, gainPot] = getPerStateHW(block);

  // get the last data point in the block
  DataType& data = block->data[block->count - 1];  


  // --- Serial Debugging Output ---
  int pp =  analogRead(SP.postGain);
  char DebugBuffer[128];
  sprintf(DebugBuffer, "SP:%d\tOff1:%d\n", pp, offsetPot1.getLevel());

  activityLED.toggle();
  Serial.print(DebugBuffer);

return;

  Serial.print(  "\t A2D:");      Serial.print(data.channels[0]);
  Serial.print(  "\t Sensor1:");  Serial.print(offsetPot1.getSensorValue());
  Serial.print(  "\t Sensor2:");  Serial.print(offsetPot2.getSensorValue());
  Serial.print(  "\t offset1:");  Serial.print(offsetPot1.getLevel());
  Serial.print(  "\t offset2:");  Serial.print(offsetPot2.getLevel());
  Serial.print(  "\t Gain:");     Serial.print(   gainPot.getLevel());
  Serial.print(  "\t Min:");      Serial.print(offsetPot2.getRunningAverage().GetMin());
  Serial.print(  "\t Max:");      Serial.print(offsetPot2.getRunningAverage().GetMax());
  Serial.println(); // must end output to be parsed correctly
*/
}








void setup() {
  if (CrashReport) USB.CrashReport(CrashReport);
  activityLED.set();

  Hardware::begin();

  offsetPot1.invert();
  offsetPot1.begin(120);

  A2D.setCallback(ProccessA2D);

  Head.setSequence( {
//    Head.ALL_OFF,
      Head.RED1,
//    Head.IR1,
//    Head.RED1 | Head.IR1,            // note; use OR ( | ) to combine states
});

  Ready = true;
  activityLED.clear();
}




void loop() {  
  // Head.setNextState();

  delay(10);

  offsetPot1.update();

  delay(10);

  USB.tick(); // output previous block, and give time for head to settle

  while (Timer.uS() < LoopPeriod_uS); // A2D.poll();
  Timer.restart();

// Hardware::tick();
//  activityLED.toggle();
}
