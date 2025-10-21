#pragma once
#include <Arduino.h>
#include "PinHelpers.h"
#include "helpers.h"
constexpr double VERSION = 0.1;

const float TickSpeed_uS = 10000;  // 10ms


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
  InputPin halt{17};

  void init() const {
    halt.init();
  }
};

struct LedPins {
  const bool Inverted = true;  // LED ON is LOW on this board

  OutputPin activity{4};
						          		//  Big puck probe  Q = photodiode                                                  // 	small probe layout  Q = photodiode    
  OutputPin IR1 {24};			//                                                 IR8							   	//
  OutputPin IR2 {25};			//  (Picture By Rob!)                     RED8               RED7					//
  OutputPin IR3 {26};			//                                                                                  //    
  OutputPin IR4 {27};			//                                                 IR9                              //            IR1
  OutputPin IR5 {28};			//                                  IR3                            IR7              //        RED4    RED1
  OutputPin IR6 {29};			//                                                                                  //      IR4    Q    IR2
  OutputPin IR7 {30};			//                                        RED1             RED9                     //        RED3    RED2
  OutputPin IR8 {31};			//                                                                                  //            IR3
  OutputPin IR9 {32};			//                               RED3              QQQQ              RED6
								      		//
  OutputPin RED1{33};			//                                        IR4              IR1
  OutputPin RED2{34};			//
  OutputPin RED3{35};			//                                 IR5                            IR2
  OutputPin RED4{36};			//                                                 RED2                 
  OutputPin RED5{37};			//
  OutputPin RED6{38};			//                                       RED4               RED5
  OutputPin RED7{39};			//                                                 IR6
  OutputPin RED8{40};			//
  OutputPin RED9{41};			

  LedPinRange all{24, 41};

  void init() {
    activity.init();

    if (Inverted) all.invert();
    all.init();
  
  }
};
