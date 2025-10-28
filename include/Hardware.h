#pragma once
#include "Setup.h"
#include "CA2D.h"
#include "CAutoPot.h"
#include <map>
#include "DataTypes.h"

struct PerStateHW {
  StateType state;
  PerStateHW(StateType state) : state(state) {}

  COffsetPot    offsetPot1{ CS.offset1, PP.primaryOffset,  50, 224, 800 };
  COffsetPot    offsetPot2{ CS.offset2, PP.preGain      , 100, 224, 800 };
  CGainPot      gainPot   { CS.gain   , PP.preGain      ,  10           };

  void begin() { 
    offsetPot1.invert();
    offsetPot2.invert();
    gainPot.invert();
    offsetPot1.begin(127); 
    offsetPot2.begin(127); 
    gainPot.begin(1); 
  }
};

struct Hardware {


  static void init();

  static void ProccessA2D(BlockType* block);

  static void tick();

};
