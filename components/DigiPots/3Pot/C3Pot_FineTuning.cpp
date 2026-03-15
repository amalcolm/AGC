#include "C3Pot.h"

int k = 0; // for testing
int centreTop = 0;
int centreBot = 0;
void C3Pot::fineTuning() {
  static int DEADBAND = 40;

  if (lockPhase) {
    k++;  if (k > 1) k = 0;

    top.setLevel(centreTop + k);
    bot.setLevel(centreBot - k);
    return;
  }

  
  int sensorValue = lastSensorValue();
  int direction = 0;

  if (sensorValue < 512 - DEADBAND) direction = +1;
  else  
  if (sensorValue > 512 + DEADBAND) direction = -1;
  
  lockPhase = (direction == 0);
  if (lockPhase) { centreTop = top.getLevel(); centreBot = bot.getLevel(); return; }

  _offsetLevel(direction);


}