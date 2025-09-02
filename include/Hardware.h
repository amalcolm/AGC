#pragma once
#include "Setup.h"
#include "CA2D.h"
#include "CAutoPot.h"

struct Hardware {

  static IntervalTimer ti_LED;
  
  static COffsetPot offsetPot1;
  static COffsetPot offsetPot2;
  static CGainPot   gainPot   ;

  static void init();

  static void ProccessA2D(CA2D::BlockType* block);

  static void tick();

};
