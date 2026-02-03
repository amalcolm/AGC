#pragma once
#include "Setup.h"
#include "CA2D.h"
#include "CAutoPot.h"
#include "DataTypes.h"
#include "CTelemetry.h"
#include "CHead.h"
#include "Config.h"

struct HWforState {
  StateType state;
  
  HWforState(StateType state) : state(state) {}

  COffsetPot    offsetPot1{ CS.offset1, SP.preGain  ,  10, 280 };
  COffsetPot    offsetPot2{ CS.offset2, SP.postGain ,  10, 280 };
  CGainPot      gainPot   { CS.gain   , SP.postGain ,  10      };

  void begin() { 
    offsetPot1.invert();
    offsetPot2.invert();
    gainPot.invert();

    offsetPot1.begin(248); 
    offsetPot2.begin(120); 
    gainPot.begin(1); 
  }

  void set() {
    offsetPot1.writeCurrentToPot();
    offsetPot2.writeCurrentToPot();
    gainPot   .writeCurrentToPot();
  }

  void update() {

    Timer.addEvent(EventKind::HW_UPDATE_START);
    offsetPot1.update();

    if (offsetPot1.inZone)
      offsetPot2.update();

    if (offsetPot2.inZone)
      gainPot.update();

    Timer.addEvent(EventKind::HW_UPDATE_COMPLETE);
  }
  
};

struct Hardware {
  static void begin();
  static void update();

  inline static SPISettings SPIsettings{4800000, MSBFIRST, SPI_MODE1};

};
