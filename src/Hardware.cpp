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
CTeleValue   TV_max(TeleGroup::HARDWARE, 5);
double _maxHWupdateDuration = 0.0;

struct S_Type  { bool setTimer = false;  bool haveRead = false;  int numUpdates = 0;
  void reset() {      setTimer = false;       haveRead = false;      numUpdates = 0; } // reset state
    
  
  void CalcNumUpdates() {
    static const double STATE_DURATION     =  CFG::STATE_DURATION_uS / 1'000'000.0; // convert to seconds
    static const double POT_OFFSET_DURATION = CFG::POT_UPDATE_OFFSET_uS / 1'000'000.0;

    double stateTime = Timer.getStateTime();
    double timeRemaining =  Timer.A2D.getRemaining_S();

    if (stateTime + _maxHWupdateDuration > STATE_DURATION) { numUpdates = 0; return; } // We don't have enough time in the state
    if (timeRemaining <= POT_OFFSET_DURATION)              { numUpdates = 0; return; } // We don't have any time left before the next A2D read
    if (_maxHWupdateDuration == 0)                         { numUpdates = 1; return; } // we have no data on update duration, default to 1

    numUpdates = (int)floor(timeRemaining / _maxHWupdateDuration); // Calculate how many updates we could fit in the remaining time based on average duration
  }
} S;  // S == instance of StateType

bool Hardware::canUpdate() {
  static double STATE_DURATION = CFG::STATE_DURATION_uS / 1'000'000.0; // convert to seconds

  return (Timer.getStateTime() + Timer.getPollDuration() < STATE_DURATION);
}

void Hardware::update() {
    if (_maxHWupdateDuration > 0) LED.RED1.on();

  TV_max.set(_maxHWupdateDuration);

  TP_Update.measure();
  TC_Update.increment();

  S.haveRead = A2D.poll();
  if (S.haveRead && !S.setTimer) { // if we have a new A2D reading and haven't set the timer for this update cycle
      Timer.HW.reset();
      S.setTimer = true;
      S.CalcNumUpdates();
  }


  if (Timer.HW.waiting()) return;

  while (S.numUpdates-- > 0) {
  
    double updateStart = Timer.getStateTime();
        getHWforState().update();  // update digital pots based on current state
    double updateEnd = Timer.getStateTime();
  
    _maxHWupdateDuration = std::max(_maxHWupdateDuration, updateEnd - updateStart);
  }

  S.reset(); // reset for next cycle

}
