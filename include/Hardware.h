#pragma once
#include "Setup.h"
#include "CA2D.h"
#include "CAutoPot.h"
#include "DataTypes.h"

struct PerStateHW {
  StateType state;
  PerStateHW(StateType state) : state(state) {}

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

  void update() {
    offsetPot1.update();

    if (offsetPot1.inZone)
      offsetPot2.update();

    if (offsetPot2.inZone)
      gainPot.update();
  }
};

struct Hardware {
  static void begin();
  static void tick();

  static SPISettings SPIsettings;
};
