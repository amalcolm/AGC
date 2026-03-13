#include "C3Pot.h"

// These are called inside update, after calling readSensor() and _checkZone()
// zone is for out of range measurements, indicating the signal is too low or too high


void C3Pot::doInit() {
  switch (zone) 
  {
    case Zone::inZone: phase = Phase::ZOOM; break;
    case Zone::Low   : top.offsetLevel(-1);   break;
    case Zone::High  : bot.offsetLevel(+1);   break;
    default: break; // avoids compilation warning
  }
}

void C3Pot::doZoom() {

  switch (zone) 
  {
    case Zone::Low   : phase = Phase::BACKOFF; return;   
    case Zone::High  : phase = Phase::BACKOFF; return;
    default: break; 
  }

  int gap = top.getLevel() - bot.getLevel();

  if (gap <= GAP_NORMAL) { phase = Phase::NORMAL; return; }

  if (lastSensorValue() < 512) 
    top.offsetLevel(-1); 
  else
    bot.offsetLevel(+1);
}


int _countDown = 0;

void C3Pot::doNormal() {
  static int halfWindow = 200;
  int direction = 0;
  int sensorValue = lastSensorValue();

  switch (zone) 
  {
    case Zone::Low   : direction = -1; break;   
    case Zone::High  : direction = +1; break;
    case Zone::inZone: 
      if (sensorValue < 512 - halfWindow) direction = -1;
      else
      if (sensorValue > 512 + halfWindow) direction = +1;
      break;
    
    default: break; // avoids compilation warning
  }

  if (direction == 0) return;

  top.offsetLevel(direction);
  bot.offsetLevel(direction);

}

void C3Pot::doBackoff() {
 
  switch (zone) 
  {
    case Zone::Low   : bot.offsetLevel(-1);   break;   
    case Zone::High  : top.offsetLevel(+1);   break;
    case Zone::inZone: phase = Phase::ZOOM; break;
    default: break; // avoids compilation warning
  }
}


