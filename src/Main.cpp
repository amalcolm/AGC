#include "Setup.h"
#include "CAutoPot.h"

ChipSelectPins CS;
ProbePointPins PP;
ButtonPins     BUT;
LedPins        LED;
Timer          timer;

Hardware HW(&CS, &PP, &BUT, &LED, &timer);

// Parameter order; ChipSelect,ProbePoint,Window,LowThresh,HighThresh
COffsetPot offsetPot1(CS.offset1, PP.primaryOffset,  50, 324, 700);
COffsetPot offsetPot2(CS.offset2, PP.preGain      , 100, 324, 700);
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


  // --- Serial Debugging Output ---
  Serial.print("\n Sensor1:");  Serial.print(offsetPot1.getSensorValue());
  Serial.print("\t Sensor2:");  Serial.print(offsetPot2.getSensorValue());
  Serial.print("\t offset1:");  Serial.print(offsetPot1.getLevel());
  Serial.print("\t offset2:");  Serial.print(offsetPot2.getLevel());
  Serial.print("\t Gain:");     Serial.print(   gainPot.getLevel());
  Serial.print("\t Min:0\t Max:1024");

  delay(5);
}
