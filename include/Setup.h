#pragma once
#include <Arduino.h>
#include "PinHelpers.h"

constexpr double VERSION = 0.1;

extern struct ChipSelectPins CS;
extern struct ProbePointPins PP;
extern struct ButtonPins     BUT;
extern struct LedPins        LED;
extern struct Hardware       HW;
extern class  CTimer         Timer;
extern class  CA2D           A2D;
extern class  CHead          Head;
extern class  CUSB           USB;


struct ChipSelectPins {
  const int offset1 = 23;
  const int offset2 = 21;
  const int gain    = 22;
  const int A2D     = 20;
};

struct ProbePointPins {
  const int primaryOffset = PIN_A1;
  const int preGain       = PIN_A0;
};

struct ButtonPins {
  InputPin< 17 > halt;

  void init() const {
    halt.init();
  }
};

struct LedPins {
  OutputPin<  4 > activity;
								//  Big puck probe  Q = photodiode                                                  // 	small probe layout  Q = photodiode    
  OutputPin< 24 > IR1;			//                                                 IR8							   	//
  OutputPin< 25 > IR2;			//  (Picture By Rob!)                     RED8               RED7					//
  OutputPin< 26 > IR3;			//                                                                                  //    
  OutputPin< 27 > IR4;			//                                                 IR9                              //            IR1
  OutputPin< 28 > IR5;			//                                  IR3                            IR7              //        RED4    RED1
  OutputPin< 29 > IR6;			//                                                                                  //      IR4    Q    IR2
  OutputPin< 30 > IR7;			//                                        RED1             RED9                     //        RED3    RED2
  OutputPin< 31 > IR8;			//                                                                                  //            IR3
  OutputPin< 32 > IR9;			//                               RED3              QQQQ              RED6
								//								
  OutputPin< 33 > RED1;			//                                        IR4              IR1
  OutputPin< 34 > RED2;			//
  OutputPin< 35 > RED3;			//                                 IR5                            IR2
  OutputPin< 36 > RED4;			//                                                 RED2                 
  OutputPin< 37 > RED5;			//
  OutputPin< 38 > RED6;			//                                       RED4               RED5
  OutputPin< 39 > RED7;			//                                                 IR6
  OutputPin< 40 > RED8;			//
  OutputPin< 41 > RED9;			

  LedPinRange<24,41> LEDs;

  void init() {
    activity.init();
    LEDs.init();
  }
};
