#include "Setup.h"
#include "Hardware.h"
#include "Helpers.h"
#include "CUSB.h"
#include "CAutoPot.h"
#include "CA2D.h"
#include "CHead.h"
#include "CTimer.h"
#include "CTelemetry.h"
#include <map>
#include "RunningAverage.h"

void Hardware::begin() {
    // Initialize all hardware components
    SPI .begin();
    USB .begin();
    BUT .begin();
    LED .begin();
    Head.begin();
    A2D .begin();

    delay(100); // Allow time for hardware to stabilize

    USB.printf("CPU Frequency: %.0f Mhz\r\n", F_CPU / 1000000.0f);
    Timer.restart();
}

CTeleCounter TC_Update{TeleGroup::HARDWARE, 1};
CTelePeriod  TP_Update{TeleGroup::HARDWARE, 2};

struct S_Type  { bool setTimer = false;  bool haveRead = false;  int numUpdates = 0;
  void reset() {      setTimer = false;       haveRead = false;      numUpdates = 0; } // reset state
    
  
  RunningAverage<double> ra{16}; 
  void CalcNumUpdates() {
    static const double MAX_AVAILABLE_TIME = (CFG::STATE_DURATION_uS - CFG::POT_UPDATE_OFFSET_uS) / 1'000'000.0;

    double timeRemaining = MAX_AVAILABLE_TIME - Timer.getStateTime();

         if (timeRemaining <= 0)   numUpdates = 0; // Force no updates if we're already over time
    else if (ra.isFull() == false) numUpdates = 1; // If we don't enough data yet, only allow one update
    else {
        double avg = ra.getAverage();
        
        numUpdates = (int)floor(timeRemaining / avg); // Calculate how many updates we could fit in the remaining time based on average duration
    }
    numUpdates = 2;
  }
} S;  // S == instance of StateType


void Hardware::update() {


  TP_Update.measure();
  TC_Update.increment();
 
  S.haveRead = A2D.poll();
  if (S     .haveRead && !S.setTimer) { // if we have a new A2D reading and haven't set the timer for this update cycle
      Timer.HW.reset();
      S.setTimer = true;
      S.CalcNumUpdates();
  }


  // ready if we haven't already updated more than once and the HW timer has elapsed
  bool isReadyToUpdate = (S.numUpdates > 0) && Timer.HW.waiting(); 

  if (!isReadyToUpdate) return;
LED.RED1.set();

  double updateStart = Timer.getStateTime();
  {
      getHWforState().update();  // update digital pots based on current state
  }
  double updateEnd = Timer.getStateTime();
  
  S.ra.add(updateEnd - updateStart);

  S.numUpdates--;
}

