#pragma once
#include <Arduino.h>
#include "PinHelpers.h"
#include "helpers.h"
#include "CMasterTimer.h"
#include <string>

extern bool TESTMODE;  // if true, uses polled A2D mode and _Callback callback by default

extern bool Ready;                // set to true once setup() is complete 

inline static std::string DEVICE_VERSION = "0.1";
inline static std::string HOST_VERSION; // set in handshake; version of software on host PC


extern struct ChipSelectPins CS; 
extern struct SensorPins     SP;
extern struct ButtonPins     BUT;
extern struct LedPins        LED;
extern class  CMasterTimer   Timer;
extern class  CA2D           A2D;
extern class  CHead          Head;
extern class  CUSB           USB;


struct ChipSelectPins {
  static constexpr int offset1 = 23;
  static constexpr int offset2 = 21;
  static constexpr int gain    = 22;
  static constexpr int A2D     = 20;
};

struct SensorPins {
  static constexpr int  preGain = PIN_A1;
  static constexpr int postGain = PIN_A0;
};

struct ButtonPins {
  InputPin halt{17};

  void begin() const {
    halt.begin();
  }
};

extern OutputPin activityLED;  // set in helpers.cpp (4)

struct LedPins {
  static constexpr bool Inverted = true;  // LED ON is LOW on this board
  static constexpr uint32_t HEAD_TO_USE = 2; 


//           bit  h1  h2     		//  Big puck probe  Q = photodiode                                                  // 	small probe layout  Q = photodiode    
  LedPin IR1 {16, 24, 31};			//  (Head 1)                                       IR8							   	            //
  LedPin IR2 {17, 25, 30};			//  (Picture By Rob!)                     RED8               RED7					          //
  LedPin IR3 {18, 26, 38};			//                                                                                  //    
  LedPin IR4 {19, 27, 36};			//                                                 IR9                              //            IR1
  LedPin IR5 {20, 28, 34};			//                                  IR3                            IR7              //        RED4    RED1
  LedPin IR6 {21, 29, 24};			//                                                                                  //      IR4    Q    IR2
  LedPin IR7 {22, 30, 25};			//                                        RED1             RED9                     //        RED3    RED2
  LedPin IR8 {23, 31, 33};			//                                                                                  //            IR3
  LedPin IR9 {24, 32, 32};			//                               RED3              QQQQ              RED6
  		    			    	      		//
  LedPin RED1{ 0, 33, 27};			//                                        IR4              IR1
  LedPin RED2{ 1, 34, 29};			//
  LedPin RED3{ 2, 35, 39};			//                                 IR5                            IR2
  LedPin RED4{ 3, 36, 26};			//                                                 RED2                 
  LedPin RED5{ 4, 37, 37};			//
  LedPin RED6{ 5, 38, 28};			//                                       RED4               RED5
  LedPin RED7{ 6, 39, 40};			//                                                 IR6
  LedPin RED8{ 7, 40, 35};			//
  LedPin RED9{ 8, 41, 41};			//

  LedPinRange all{24, 41};

  void begin() {
    Pins::setHead(HEAD_TO_USE);

    activityLED.begin();

    if (Inverted) all.invert();
    all.begin();

    all.clear();

  }
};
