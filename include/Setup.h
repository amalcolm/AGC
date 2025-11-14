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
  static constexpr int offset1 = 23;
  static constexpr int offset2 = 21;
  static constexpr int gain    = 22;
  static constexpr int A2D     = 20;
};

struct ProbePointPins {
  static constexpr int primaryOffset = PIN_A1;
  static constexpr int preGain       = PIN_A0;
};

struct ButtonPins {
  InputPin halt{17};

  void init() const {
    halt.init();
  }
};

extern OutputPin activityLED;  // set in helpers.cpp (4)

struct LedPins {
  static constexpr bool Inverted = true;  // LED ON is LOW on this board

						   //h1  h2            		//  Big puck probe  Q = photodiode                                                  // 	small probe layout  Q = photodiode    
  OutputPin IR1 {24, 31};			//                                                 IR8							   	//
  OutputPin IR2 {25, 30};			//  (Picture By Rob!)                     RED8               RED7					//
  OutputPin IR3 {26, 38};			//                                                                                  //    
  OutputPin IR4 {27, 36};			//                                                 IR9                              //            IR1
  OutputPin IR5 {28, 34};			//                                  IR3                            IR7              //        RED4    RED1
  OutputPin IR6 {29, 24};			//                                                                                  //      IR4    Q    IR2
  OutputPin IR7 {30, 25};			//                                        RED1             RED9                     //        RED3    RED2
  OutputPin IR8 {31, 33};			//                                                                                  //            IR3
  OutputPin IR9 {32, 32};			//                               RED3              QQQQ              RED6
				    				      		//
  OutputPin RED1{33, 27};			//                                        IR4              IR1
  OutputPin RED2{34, 29};			//
  OutputPin RED3{35, 39};			//                                 IR5                            IR2
  OutputPin RED4{36, 26};			//                                                 RED2                 
  OutputPin RED5{37, 37};			//
  OutputPin RED6{38, 28};			//                                       RED4               RED5
  OutputPin RED7{39, 40};			//                                                 IR6
  OutputPin RED8{40, 35};			//
  OutputPin RED9{41, 41};			

  LedPinRange all{24, 41};

  void init() {
    activityLED.init();

    if (Inverted) all.invert();
    all.init();
  
  }
};
