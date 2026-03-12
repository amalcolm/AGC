#include "C3Pot.h"

void C3Pot::doInit() {  // Called after readSensor() and checkZone() in update()
  switch (zone) 
  {
    case Zone::inZone: phase = Phase::ZOOM; break;
    case Zone::Low   : top.offsetLevel(-1);   break;
    case Zone::High  : bot.offsetLevel(+1);   break;
    default: break; // avoids compilation warning
  }
}

void C3Pot::doZoom() {  // Called after readSensor() and checkZone() in update()

  switch (zone) 
  {
    case Zone::Low   : phase = Phase::BACKOFF; return;   
    case Zone::High  : phase = Phase::BACKOFF; return;
    default: break; // avoids compilation warning
  }

  int gap = top.getLevel() - bot.getLevel();

  if (gap < GAP_NORMAL) { phase = Phase::NORMAL; return; }

  if (_lastSensorValue < 512) 
    top.offsetLevel(-1); 
  else
    bot.offsetLevel(+1);
}


int _countDown = 0;

void C3Pot::doNormal() {

  switch (zone) 
  {
    case Zone::Low   : if (_countDown-- <= 0) phase = Phase::BACKOFF; return;   
    case Zone::High  : if (_countDown-- <= 0) phase = Phase::BACKOFF; return;
    case Zone::inZone: return;   // we're good, stay in normal
    default: break; // avoids compilation warning
  }

  int direction = zone == Zone::Low ? -1 : +1;

  top.offsetLevel(direction);
  bot.offsetLevel(direction);

  _countDown = 8; 
 
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
