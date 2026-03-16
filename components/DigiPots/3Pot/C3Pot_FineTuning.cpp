#include "C3Pot.h"
#include "CUSB.h"

int k = 0; // for testing
int centreTop = 0;
int centreBot = 0;
int centreMid = 0;
void C3Pot::fineTuning() {
  static constexpr int SENSOR_DEADBAND = 3;
  static constexpr int SENSOR_LOW      = 512 - SENSOR_DEADBAND;
  static constexpr int SENSOR_HIGH     = 512 + SENSOR_DEADBAND;

  static constexpr int MID_STEP = 150;

  static constexpr int WIPER_LOW  = (256 - MID_STEP) / 2;
  static constexpr int WIPER_HIGH =  256 - WIPER_LOW;

  int direction = 0;
  int wiperLevel = mid.getLevel();

  if (wiperLevel < WIPER_LOW ) direction = +1;
  else
  if (wiperLevel > WIPER_HIGH) direction = -1;

  if (direction != 0) {
    top.offsetLevel(direction);
    bot.offsetLevel(direction);
    mid.offsetLevel(direction * MID_STEP);
    return;
  }

  
  int sensorValue = lastSensorValue();

  if (sensorValue < SENSOR_LOW ) mid.offsetLevel(+1);
  else  
  if (sensorValue > SENSOR_HIGH) mid.offsetLevel(-1);

  
}