#include "Setup.h"
#include "Helpers.h"
#include "CAutoPot.h"


// Parameter order; ChipSelect,ProbePoint,Window,LowThresh,HighThresh
COffsetPot offsetPot1(CS.offset1, PP.primaryOffset,  50, 224, 800);
COffsetPot offsetPot2(CS.offset2, PP.preGain      , 100, 224, 800);
CGainPot   gainPot   (CS.gain,    PP.preGain      ,  10          );

void setup() {
  HW.init();

  LED.all.deactivate();
  digitalWrite(LED.RED2, LOW); // Example: Turn on a specific LED
  
  offsetPot1.begin(138);
  offsetPot2.begin(142);
  gainPot.invert();
  gainPot.begin(1);
  timer.restart();
}


void loop() {
  const float uS_delay = 9000;

  while (timer.uS() < uS_delay);
 
  timer.restart();
  LED.activity.toggle();


  offsetPot1.update();
  offsetPot2.update();

//if (offsetPot1.inZone) {
    gainPot.update();
//}

  auto avg1 = offsetPot1.getRunningAverage().GetAverage() - 900;
  auto avg2 = offsetPot2.getRunningAverage().GetAverage() - 512;

  // --- Serial Debugging Output ---
  Serial.print("\n Sensor1:");  Serial.print(offsetPot1.getSensorValue()-avg1);
  Serial.print("\t Sensor2:");  Serial.print(offsetPot2.getSensorValue()-avg2);
//  Serial.print("\t offset1:");  Serial.print(offsetPot1.getLevel());
//  Serial.print("\t offset2:");  Serial.print(offsetPot2.getLevel());
//  Serial.print("\t Gain:");     Serial.print(   gainPot.getLevel());
  Serial.print("\t Min:");      Serial.print(offsetPot2.getRunningAverage().GetMin() - avg2);
  Serial.print("\t Max:");      Serial.print(offsetPot2.getRunningAverage().GetMax() - avg2);

  delay(5);
}
