#include "Setup.h"
#include "Hardware.h"
#include "CTimer.h"


void setup() {
  Hardware::init();
}


void loop() {
  const float uS_delay = 1000;  // 1ms

  while (Timer.uS() < uS_delay);
 
  Timer.restart();
  LED.activity.toggle();


  delay(5);
}
