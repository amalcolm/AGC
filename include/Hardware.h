#pragma once
#include <tuple>
#include <utility>
#include "Setup.h"
#include "CA2D.h"
#include "CAutoPot.h"
#include "DataTypes.h"
#include "CTelemetry.h"
#include "CHead.h"
#include "Config.h"

struct HWforState {
  public:
    StateType state;
    
    HWforState(StateType state) : state(state) {}

    C3Pot         TIA{CS.TIA_TOP, CS.TIA_BOT, CS.TIA_MID, SP.TIA};
    COffsetPot    offsetPot2{ CS.offset2, SP.Final ,  2, 400 };   // 280 normal
    CGainPot      gainPot   { CS.gain   , SP.Final ,  2, 400 };   // 100 normal };

    bool begun = false;
    void begin() {
      gainPot   .invert();

      TIA.begin();

      offsetPot2.begin(128); 
      gainPot   .begin(  0);
      
      begun = true;
    }

    // write current state of hardware instances to hardware devices
    void set() { if (!Ready) return; else if (!begun) begin(); // ensure begin is called before first use, but only once
      TIA.writeCurrentToPot();
      offsetPot2.writeCurrentToPot();
      gainPot   .writeCurrentToPot();
    }

    // update hardware instances based on current sensor readings, and write to hardware if needed
    void update() { if (!Ready) return; else if (!begun) begin();

      Timer.addEvent(EventKind::HW_UPDATE_START);
      TIA.update();

      if (TIA.inZone)
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
