#pragma once
#include "SPI.h"
#include "Timer.h"
#include "PinHelpers.h"

struct ChipSelectPins {
  const int offset1 = 23;
  const int offset2 = 21;
  const int gain    = 22;
  const int A2D     = 20;
};

struct ProbePointPins {
  const int primaryOffset = A1;
  const int preGain       = A0;
};

struct ButtonPins {
  InputPin<17> halt;

  void init() const {
    halt.init();
  }
};

struct LedPins {
  OutputPin<4> activity;
  	
	              													//	big puck probe  Q = photodiode                                                    // 	small probe layout  Q = photodiode    
	const int IR1  = 24;										//                                                 IR8
	const int IR2  = 25;										//  (Picture By Rob!)                     RED8               RED7
	const int IR3  = 26;										//                                                                                    //    
	const int IR4  = 27;										//                                                 IR9                                //            IR1
	const int IR5  = 28;										//                                  IR3                            IR7                //        RED4    RED1
	const int IR6  = 29;										//                                                                                    //      IR4    Q    IR2
	const int IR7  = 30;										//                                        RED1             RED9                       //        RED3    RED2
	const int IR8  = 31;										//                                                                                    //            IR3
	const int IR9  = 32;										//                               RED3              QQQQ              RED6
																//             
	const int RED1 = 33;										//                                        IR4              IR1
	const int RED2 = 34;										//
	const int RED3 = 35;										//                                 IR5                            IR2
	const int RED4 = 36;										//                                                 RED2                 
	const int RED5 = 37;										//
	const int RED6 = 38;										//                                       RED4               RED5
	const int RED7 = 39;										//                                                 IR6
	const int RED8 = 40;     
	const int RED9 = 41;

  LedPinRange all{24, 41};

  void init() const {
    activity.init();
    all.init();
  }
};


struct Hardware {
  ChipSelectPins* const CS;
  ProbePointPins* const PP;
  ButtonPins    * const BUT;
  LedPins       * const LED;
  Timer         * const timer;

  Hardware(ChipSelectPins* cs, ProbePointPins *pp, ButtonPins *but, LedPins *led, Timer* timer)
    : CS(cs), PP(pp), BUT(but), LED(led), timer(timer) {
    };

  void init() const {
    Serial.begin(57600*16);

    SPI.begin();

    BUT->init();
    LED->init();

    Serial.printf("CPU Frequency: %.0f Mhz\n", F_CPU / 1000000.0f);
    timer->restart();
  }

};
