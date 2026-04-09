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
    COpAmp        OpAmp{CS.offset2, CS.gain, SP.Final};

    bool offsetsChanged = true;

    bool begun = false;
    void begin() {

      TIA.begin();
      OpAmp.begin();

      begun = true;
    }

    // write current state of hardware instances to hardware devices
    void set() { if (!Ready) return; else if (!begun) begin(); // ensure begin is called before first use, but only once
      TIA.set();
      OpAmp.set();
    }

    // update hardware instances based on current sensor readings, and write to hardware if needed
    void update() { if (!Ready) return; else if (!begun) begin();

      Timer.addEvent(EventKind::HW_UPDATE_START);
      TIA.update();

      if (TIA.inZone)
        OpAmp.update();

      Timer.addEvent(EventKind::HW_UPDATE_COMPLETE);
    }

};

extern HWforState* HW;

struct Hardware {
  inline static SPISettings SPIsettings{4800000, MSBFIRST, SPI_MODE1};

  static void begin();

  static bool canUpdate();
  static void update();


};
