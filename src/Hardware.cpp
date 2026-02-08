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
CTeleValue   TV_maxDur(TeleGroup::HARDWARE, 3);
double _maxHWdur = 0.0;

struct S_Type  { bool setTimer = false;  bool haveRead = false;  int numUpdates = 0;
  void reset() {      setTimer = false;       haveRead = false;      numUpdates = 0; } // reset state
    
  
  void CalcNumUpdates() {
    static const double STATE_DURATION     =  CFG::STATE_DURATION_uS / 1'000'000.0; // convert to seconds
    static const double POT_OFFSET_DURATION = CFG::POT_UPDATE_OFFSET_uS / 1'000'000.0;

    double remainingInState = STATE_DURATION - Timer.getStateTime();
    double remainingToNextA2D = Timer.A2D.getRemaining_S();
    double usable = std::min(remainingInState, remainingToNextA2D) - POT_OFFSET_DURATION;
    if (usable <= 0)  { numUpdates = 0; return; } // We don't have any time left before the next A2D read

    if (_maxHWdur == 0) 
      numUpdates = 1; // we have no data on update duration, default to 1
    else
      numUpdates = (int)floor(usable / _maxHWdur); // Calculate how many updates we could fit in the remaining time based on average duration
  }
} S;  // S == instance of StateType

bool Hardware::canUpdate() {
  static double STATE_DURATION = CFG::STATE_DURATION_uS / 1'000'000.0; // convert to seconds

  return (Timer.getStateTime() + Timer.getPollDuration() < STATE_DURATION);
}

CTeleValue TV_Test(TeleGroup::HARDWARE, 0xFF);
double lastMark = 0.0;
void Hardware::update() {

  TP_Update.measure();
  TC_Update.increment();
  TV_maxDur.set(_maxHWdur * 1'000'000.0); // in microseconds

  S.haveRead = A2D.poll();
  if (S.haveRead && !S.setTimer) { // if we have a new A2D reading and haven't set the timer for this update cycle
      if (lastMark > 0.0)
        TV_Test.set((Timer.getConnectTime() - lastMark) * 1'000'000.0); // in microseconds
      lastMark = Timer.getConnectTime();
      Timer.HW.reset();
      S.setTimer = true;
      S.CalcNumUpdates();
  }


  if (Timer.HW.waiting()) return;

  while (S.numUpdates-- > 0) {
  
    double updateStart = Timer.getStateTime();
        getHWforState().update();  // update digital pots based on current state
    double updateEnd = Timer.getStateTime();
  
    _maxHWdur = std::max(_maxHWdur, updateEnd - updateStart);
  }

  S.reset(); // reset for next cycle

}

