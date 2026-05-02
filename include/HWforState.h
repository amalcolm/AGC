#pragma once
#include "Setup.h"
#include "DataTypes.h"
#include "CAutoPot.h"
#include "CMasterTimer.h"

struct HWforState {
  public:

    struct HWflags {
      bool holdStage1 = false;
      bool holdStage2 = false;

      bool offsetsChanged = true;
      bool begun = false;

      void dbg();
    } flags;

    StateType state;
    
    HWforState(StateType state) : state(state) {}

    CStage1        Stage1{CS.Stage1_TOP, CS.Stage1_BOT, CS.Stage1_MID, SP.Stage1};
    CStage2        OpAmp{CS.offset2, CS.gain, SP.Final};

    void begin() {

      Stage1.begin();
      OpAmp.begin();

      flags.begun = true;
    }

    // write current state of hardware instances to hardware devices
    void set() { if (!Ready) return; else if (!flags.begun) begin(); // ensure begin is called before first use, but only once
      Stage1.set();
      OpAmp.set();
    }

    // update hardware instances based on current sensor readings, and write to hardware if needed
    void update() { if (!Ready) return; else if (!flags.begun) begin();

      Timer.addEvent(EventKind::HW_UPDATE_START);

      Stage1.update();

      if (Stage1.inZone)
        OpAmp.update();
      else
        OpAmp.inZone = false;
    
      flags.dbg();  // defined in _DBG.cpp
      
      Timer.addEvent(EventKind::HW_UPDATE_COMPLETE);
    }

};

extern HWforState* HW;
