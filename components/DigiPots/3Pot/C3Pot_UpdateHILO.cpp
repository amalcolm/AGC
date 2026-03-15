#include "C3Pot.h"

// This is called inside update, after calling readSensor() and _checkZone()
// zone is for out of range measurements, indicating the signal is too low or too high

void C3Pot::updateHILO() {

  if (lockPhase) return;
 
  int gap = top.getLevel() - bot.getLevel();

  switch (phase) 
  {
    // initialization; zoom from min/max to the point where we see the signal, then switch to zoom mode
    case Phase::INIT: 
      switch (zone) 
      {
        case Zone::inZone: phase = Phase::ZOOM; return;
        case Zone::Low   : top.offsetLevel(-1); return;
        case Zone::High  : bot.offsetLevel(+1); return;
        default: break; // avoids compilation warning
      }
      break;

      
    // zoom mode: if we're out of range, back off;
    //            if we're in range, check the gap between top and bot; if it's small, switch to slide mode; otherwise keep zooming
    case Phase::ZOOM:
      switch (zone) 
      {
        case Zone::Low   : phase = Phase::BACKOFF; return;   
        case Zone::High  : phase = Phase::BACKOFF; return;
        default: break; 
      }

      if (gap <= GAP_NORMAL) { phase = Phase::NORMAL; return; }

      if (lastSensorValue() < 512) 
        top.offsetLevel(-1); 
      else
        bot.offsetLevel(+1);
    
      break;


    // backoff mode: if we're out of range, back off;
    //               if we're in range, swtich to zoom again
    case Phase::BACKOFF:
      switch (zone) {
        case Zone::inZone: phase = Phase::ZOOM;  return;
        default: 
          bot.offsetLevel(-1);
          top.offsetLevel(+1);
          break;  
      }
      break;

    
    case Phase::NORMAL:
      if (zone != Zone::inZone) { mid.reset(128); phase = Phase::BACKOFF; return; }
      break;

    default: break; // avoids compilation warning
  }
}

