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

    struct DBG {
      int started = false;
      bool settling = false;
      bool holdStage1 = false;
      bool holdStage2 = false;
      double markTime = 0;

      bool toggle = false;

      void run(struct HWforState* hw) {

        started |= hw->Stage1.inZone;

        if (!started) return;
        
        double now = Timer.getConnectTime();
        bool justStarted = !started && now > 5.0;
        started = now > 5.0;
        holdStage1 = now > 10;
        holdStage2 = holdStage1;

        if (justStarted)
        {
          hw->Stage1.top.offsetLevel(+1);
          hw->Stage1.bot.offsetLevel(+ 1);
          delayMicroseconds(10);
        }

        if (started) {
          if (now - markTime > 1.0) {
            markTime = now;
            toggle = !toggle;

            if (toggle) {
              hw->Stage1.top.offsetLevel(+1);
              hw->Stage1.bot.offsetLevel(+1);
            } else {
              hw->Stage1.top.offsetLevel(-1);
              hw->Stage1.bot.offsetLevel(-1);
            }
//            hw->Stage1.mid.offsetLevel(toggle ? +24 : -24);
          }
        }
      }

    } dbg;
    StateType state;
    
    HWforState(StateType state) : state(state) {}

    CStage1        Stage1{CS.Stage1_TOP, CS.Stage1_BOT, CS.Stage1_MID, SP.Stage1};
    CStage2        OpAmp{CS.offset2, CS.gain, SP.Final};

    bool offsetsChanged = true;

    bool begun = false;
    void begin() {

      Stage1.begin();
      OpAmp.begin();

      begun = true;
    }

    // write current state of hardware instances to hardware devices
    void set() { if (!Ready) return; else if (!begun) begin(); // ensure begin is called before first use, but only once
      Stage1.set();
      OpAmp.set();
    }

    // update hardware instances based on current sensor readings, and write to hardware if needed
    void update() { if (!Ready) return; else if (!begun) begin();

      Timer.addEvent(EventKind::HW_UPDATE_START);
      if (dbg.holdStage1)
        Stage1.readSensor();
      else
        Stage1.update();

      if (Stage1.inZone == false)
        OpAmp.inZone = false;
      else
        if (dbg.holdStage2)
          OpAmp.readSensor();
        else
          OpAmp.update();
            
      dbg.run(this);
      
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
