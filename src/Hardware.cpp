#include "Setup.h"
#include "Hardware.h"

#include "CUSB.h"
#include "CAutoPot.h"
#include "CA2D.h"
#include "CHead.h"
#include "CTimer.h"
#include <map>


void Hardware::init() {
    // Initialize all hardware components
    USB .init();
    SPI .begin();
    BUT .init();
    LED .init();
    Head.init();
    A2D .init()->setCallback(ProccessA2D);

    Serial.printf("CPU Frequency: %.0f Mhz\r\n", F_CPU / 1000000.0f);
    Timer.restart();


}

void Hardware::ProccessA2D(CA2D::BlockType* block) {
    if (block->data == NULL) return;
    USB.buffer(block);
}


void Hardware::tick() {
  static std::map<CHead::StateType, PerStateHW> stateMap;

  const auto state = Head.getState();
  auto  [it, inserted] = stateMap.try_emplace(state, state);
  auto& [_, offsetPot1, offsetPot2, gainPot] = it->second;
  if (inserted) {
      offsetPot1.begin(127);
      offsetPot2.begin(127);
      gainPot.begin(1);
  }

  offsetPot1.update();
  offsetPot2.update();
  if (offsetPot1.inZone) gainPot.update();  

  auto avg1 = offsetPot1.getRunningAverage().GetAverage() - 900;
  auto avg2 = offsetPot2.getRunningAverage().GetAverage() - 512;
return;
  // --- Serial Debugging Output ---
  Serial.print("\r\n Sensor1:");  Serial.print(offsetPot1.getSensorValue()-0*avg1);
  Serial.print(  "\t Sensor2:");  Serial.print(offsetPot2.getSensorValue()-0*avg2);
  Serial.print(  "\t offset1:");  Serial.print(offsetPot1.getLevel());
  Serial.print(  "\t offset2:");  Serial.print(offsetPot2.getLevel());
  Serial.print(  "\t Gain:");     Serial.print(   gainPot.getLevel());
  Serial.print(  "\t Min:");      Serial.print(offsetPot2.getRunningAverage().GetMin() - 0*avg2);
  Serial.print(  "\t Max:");      Serial.print(offsetPot2.getRunningAverage().GetMax() - 0*avg2);

}