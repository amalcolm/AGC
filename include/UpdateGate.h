#pragma once
#include "CA2D.h"
#include "Setup.h"   
#include "Hardware.h"
#include "Helpers.h"

struct UpdateGate {
  bool setTimer = false;
  bool haveRead = false;
  int lastUpdate  = 0;
  int  numUpdates = 0;

  void reset() {
    setTimer = false;
    haveRead = false;
    numUpdates = 0;
    lastUpdate = 0;
  }

  void poll() {
    haveRead = A2D.poll();
    if (haveRead && !setTimer) {
      Timer.HW.reset();
      setTimer = true;
      numUpdates = 0;
      lastUpdate = 0;
    }
  }

  bool readyToUpdate() const {
    if (numUpdates > 1) return false;
   
    if (Timer.HW.waiting()) return false;

    getHWforState().update();  // update pots

    return true;
  }

  void markUpdated() {
    numUpdates++;
 
    if (numUpdates <= 1) {
      Timer.HW.reset();
      setTimer = true;
    } else {
      setTimer = false;
    }
  }
};
