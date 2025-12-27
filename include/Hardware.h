#pragma once
#include "Setup.h"
#include "CA2D.h"
#include "CAutoPot.h"
#include "DataTypes.h"
#include "CTelemetry.h"
#include "CHead.h"
#include "Config.h"

struct PerStateHW {
  StateType state;
  PerStateHW(StateType state) : state(state) {}

  COffsetPot    offsetPot1{ CS.offset1, SP.preGain  ,  10, 300 };
  COffsetPot    offsetPot2{ CS.offset2, SP.postGain ,  10, 300 };
  CGainPot      gainPot   { CS.gain   , SP.postGain ,  10      };

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
    uint16_t sequenceNumber = Head.getSequenceNumber() << 8;
  //  CTeleTimer TT_Offset1{TeleGroup::DIGIPOTS, (uint16_t)(0x01 | sequenceNumber)};
  //  CTeleTimer TT_Offset2{TeleGroup::DIGIPOTS, (uint16_t)(0x02 | sequenceNumber)};
  //  CTeleTimer TT_Gain   {TeleGroup::DIGIPOTS, (uint16_t)(0x03 | sequenceNumber)};
  } tele;

  void update() {

//  tele.TT_Offset1.start();
    offsetPot1.update();
//  tele.TT_Offset1.stop();

//  tele.TT_Offset2.start();
    if (offsetPot1.inZone)
      offsetPot2.update();
//  tele.TT_Offset2.stop();

//  tele.TT_Gain.start();
    if (offsetPot2.inZone)
      gainPot.update();
//  tele.TT_Gain.stop();
  }
  
};

struct Hardware {
  static void begin();
  static void update();

  inline static SPISettings SPIsettings{4800000, MSBFIRST, SPI_MODE1};
  inline static CTimedGate  gate{2.0/CFG::READING_SPEED_Hz};  // 250 Hz gate; also ensures A2D is not busy 

};
