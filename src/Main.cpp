#include "Setup.h"
#include "Hardware.h"
#include "CTimer.h"
#include "CHead.h"
#include "CUSB.h"

bool TESTMODE = false;  // if true, uses polled A2D mode and ProccessA2D callback by default

constexpr double LoopPeriod_uS = 20000;  // 20ms

// ProcessA2D: Callback to process A2D data blocks for debugging
void ProccessA2D(BlockType* block) {  if (!TESTMODE || block == nullptr || block->count == 0) return;

  // if we are outputting the A2D data, skip the debug below
  A2D.outputDebugBlock = false;  if (A2D.outputDebugBlock) return; 

  // get hardware for the block's state
  auto& [state, offsetPot1, offsetPot2, gainPot] = getPerStateHW(block);

  // get the last data point in the block
  DataType& data = block->data[block->count - 1];  

  // Output debug info to Serial

  Serial.print(     "A2D:");      Serial.print(data.channels[0]);
  Serial.print(  "\t Sensor1:");  Serial.print(offsetPot1.getSensorValue());
  Serial.print(  "\t Sensor2:");  Serial.print(offsetPot2.getSensorValue());
  Serial.print(  "\t offset1:");  Serial.print(offsetPot1.getLevel());
  Serial.print(  "\t offset2:");  Serial.print(offsetPot2.getLevel());
  Serial.print(  "\t Gain:");     Serial.print(   gainPot.getLevel());
  Serial.print(  "\t Min:");      Serial.print(offsetPot2.getRunningAverage().GetMin());
  Serial.print(  "\t Max:");      Serial.print(offsetPot2.getRunningAverage().GetMax());
  Serial.println(); // must end output to be parsed correctly

}








void setup() {
  if (CrashReport) USB.SendCrashReport(CrashReport);
  activityLED.set();

  A2D.setCallback(ProccessA2D);

  Hardware::begin();

  Head.setSequence( {
//    Head.ALL_OFF,
      Head.RED8,
//      Head.IR8,
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

  while (Timer.uS() < LoopPeriod_uS) Hardware::update();  // update hardware until period elapses
  Timer.restart();

  Head.unsetReady();


  Tele(CTelemetry::Group::A2D     , CA2D::TeleKind::COUNT, 0, getCounter(0));
  Tele(CTelemetry::Group::HARDWARE, CA2D::TeleKind::COUNT, 1, getCounter(1));
  Tele(CTelemetry::Group::A2D     , CA2D::TeleKind::COUNT, 2, getCounter(2));
  Tele(CTelemetry::Group::PROGRAM , CA2D::TeleKind::COUNT, 5, getCounter(5));

  Tele(CTelemetry::Group::PROGRAM , CA2D::TeleKind::COUNT, 60, getCounter(60));

  activityLED.toggle();
}
