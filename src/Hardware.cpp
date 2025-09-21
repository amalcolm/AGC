#include "Setup.h"
#include "Hardware.h"
#include "Helpers.h"
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

    delay(100); // Allow time for hardware to stabilize

    USB.printf("CPU Frequency: %.0f Mhz\r\n", F_CPU / 1000000.0f);
    Timer.restart();
}

void Hardware::ProccessA2D(CA2D::BlockType* block) {
    if (block->data == NULL) return;
    Serial.print("\r\nA2D:"); Serial.println(block->data->at(0).Channels[0] / 6000);
    USB.buffer(block);
}


void Hardware::tick() {

  auto& [state, offsetPot1, offsetPot2, gainPot] = getPerStateHW();

  offsetPot1.update();
  offsetPot2.update();
  if (offsetPot1.inZone) gainPot.update(); 

  CA2D::DataType data = A2D.getData();


  auto avg1 = offsetPot1.getRunningAverage().GetAverage() - 900;
  auto avg2 = offsetPot2.getRunningAverage().GetAverage() - 512;

  // --- Serial Debugging Output ---
  Serial.print("\r\n A2D:");      Serial.print(data.Channels[0]/6000);
  Serial.print(  "\t Sensor1:");  Serial.print(offsetPot1.getSensorValue()-0*avg1);
  Serial.print(  "\t Sensor2:");  Serial.print(offsetPot2.getSensorValue()-0*avg2);
  Serial.print(  "\t offset1:");  Serial.print(offsetPot1.getLevel());
  Serial.print(  "\t offset2:");  Serial.print(offsetPot2.getLevel());
  Serial.print(  "\t Gain:");     Serial.print(   gainPot.getLevel());
  Serial.print(  "\t Min:");      Serial.print(offsetPot2.getRunningAverage().GetMin() - 0*avg2);
  Serial.print(  "\t Max:");      Serial.print(offsetPot2.getRunningAverage().GetMax() - 0*avg2);

}