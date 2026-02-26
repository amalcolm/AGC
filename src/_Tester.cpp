#include "Setup.h"
#include "Hardware.h"
#include "CMasterTimer.h"
#include "CHead.h"
#include "CUSB.h"
#include "Config.h"
#include "Helpers.h"

HWforState* HW;

void _setup() {
  activityLED.set();

  Hardware::begin();

  Ready = true;

  HW = &getHWforState(Head.RED1);

  activityLED.clear();
}


void _loop() {
  while (!Serial) yield(); // wait for Serial to be ready before outputting debug info



  USB.printf("offet1sensor:%d  offset1:%d\n", HW->offsetPot1.lastSensorValue(), HW->offsetPot1.getLevel());
  HW->update();
  delay(1000);
  activityLED.toggle();             // Indicate activity on LED
}
