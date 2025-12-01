#include "Setup.h"
#include "Hardware.h"
#include "CTimer.h"
#include "CHead.h"
#include "CUSB.h"

const float TickSpeed_uS = 20000;  // 20ms

const std::vector<StateType> sequence = {
//    Head.ALL_OFF,
      Head.RED1, Head.RED2,
//    Head.IR1,
//    Head.RED1 | Head.IR1,            // note; use OR ( | ) to combine states
};


// ProcessA2D: Callback to process A2D data blocks for debugging
void ProccessA2D(BlockType* block) { if (!TESTMODE || block == nullptr || block->count == 0) return;

  A2D.outputDebugBlock = false;

  if (A2D.outputDebugBlock) return;  // if we are outputting the A2D data, skip this
  

  auto& [state, offsetPot1, offsetPot2, gainPot] = getPerStateHW(block);

  DataType data = block->data[block->count - 1];

  auto avg1 = offsetPot1.getRunningAverage().GetAverage() - 900;
  auto avg2 = offsetPot2.getRunningAverage().GetAverage() - 512;

  
  // --- Serial Debugging Output ---
  Serial.print(     "A2D:");      Serial.print(data.channels[0]);
  Serial.print(  "\t Sensor1:");  Serial.print(offsetPot1.getSensorValue()-0*avg1);
  Serial.print(  "\t Sensor2:");  Serial.print(offsetPot2.getSensorValue()-0*avg2);
  Serial.print(  "\t offset1:");  Serial.print(offsetPot1.getLevel());
  Serial.print(  "\t offset2:");  Serial.print(offsetPot2.getLevel());
  Serial.print(  "\t Gain:");     Serial.print(   gainPot.getLevel());
  Serial.print(  "\t Min:");      Serial.print(offsetPot2.getRunningAverage().GetMin() - 0*avg2);
  Serial.print(  "\t Max:");      Serial.print(offsetPot2.getRunningAverage().GetMax() - 0*avg2);
  Serial.println(); // must end output to be parsed correctly
  
}








void setup() {
  if (CrashReport) USB.CrashReport(CrashReport);
  activityLED.set();

  Hardware::begin();

  USB.setMode(CUSB::ModeType::BLOCKDATA);
  if (!TESTMODE) A2D.setCallback(ProccessA2D);

  Head.setSequence(sequence);

  Ready = true;
  activityLED.set();
}




void loop() {
  Head.setNextState();                                                       if (TESTMODE)  { BlockType block; ProccessA2D(&block); }  
  USB.tick(); // output previous block, and give time for head to settle

  while (Timer.uS() < TickSpeed_uS) A2D.poll();
  Timer.restart();

  Hardware::tick();
  activityLED.toggle();
}
