#pragma once
#include "Setup.h"
#include "CA2D.h"
#include "CAutoPot.h"
#include "DataTypes.h"
#include "CTelemetry.h"

struct PerStateHW {
  StateType state;
  PerStateHW(StateType state) : state(state) {
    USB.printf("PerStateHW constructed for state 0x%08X at %p\n", state, this);
  }

  COffsetPot    offsetPot1{ CS.offset1, SP.preGain  ,  50, 212, 812 };
  COffsetPot    offsetPot2{ CS.offset2, SP.postGain , 100, 224, 800 };
  CGainPot      gainPot   { CS.gain   , SP.postGain ,  10           };

  void begin() { 
    offsetPot1.invert();
    offsetPot2.invert();
    gainPot.invert();

    offsetPot1.begin(120); 
    offsetPot2.begin(120); 
    gainPot.begin(1); 
  }

  void set() {
    offsetPot1.writeCurrentToPot();
    offsetPot2.writeCurrentToPot();
    gainPot   .writeCurrentToPot();
  }

  struct Telemetry {
    CTeleTimer TT_Offset1{TeleGroup::DIGIPOTS, 0x01};
    CTeleTimer TT_Offset2{TeleGroup::DIGIPOTS, 0x02};
    CTeleTimer TT_Gain   {TeleGroup::DIGIPOTS, 0x03};
  } tele;

  void update() {

    tele.TT_Offset1.start();
    {
      offsetPot1.update();
    }
    tele.TT_Offset1.stop();

    tele.TT_Offset2.start();
    {
    if (offsetPot1.inZone)
      offsetPot2.update();
    }
    tele.TT_Offset2.stop();

    tele.TT_Gain.start();
    {
      if (offsetPot2.inZone)
      gainPot.update();
    }
    tele.TT_Gain.stop();
  }
  
};

struct Hardware {
  static void begin();
  static void update();

  static SPISettings SPIsettings;
};
