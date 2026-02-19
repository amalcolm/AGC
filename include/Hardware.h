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

  CDigiPot      offset1_hi{ CS.offset1Upper};
  CDigiPot      offset1_lo{ CS.offset1Lower};
  COffsetPot    offsetPot1{ CS.offset1, SP.preGain  ,  5, 280 };
  COffsetPot    offsetPot2{ CS.offset2, SP.postGain ,  5, 280 };
  CGainPot      gainPot   { CS.gain   , SP.postGain ,  5      };

  bool begun = false;
  void begin() { 
    offsetPot1.invert();
    offsetPot2.invert();
    gainPot.invert();

    offsetPot1.begin(248); 
    offsetPot2.begin(120); 
    gainPot.begin(1);
    
    begun = true;
  }

  // write current state of hardware instances to hardware devices
  void set() { if (!Ready) return; else if (!begun) begin(); // ensure begin is called before first use, but only once
    offset1_hi.writeCurrentToPot();
    offset1_lo.writeCurrentToPot();
    offsetPot1.writeCurrentToPot();
    offsetPot2.writeCurrentToPot();
    gainPot   .writeCurrentToPot();
  }

  // update hardware instances based on current sensor readings, and write to hardware if needed
  void update() { if (!Ready) return; else if (!begun) begin();

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
  inline static SPISettings SPIsettings{4800000, MSBFIRST, SPI_MODE1};

  static void begin();

  static bool canUpdate();
  static void update();


};
