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
    SPI .begin();  // initialise SPI

    // Initialize all our hardware components
    USB .begin().printf("CPU Frequency: %.0f Mhz\r\n", F_CPU / 1000000.0f);

    
    BUT .begin();
    LED .begin();
    Head.begin();
    A2D .begin();

    delay(1); // Allow time for hardware to stabilize (ample)

    
    // ensure A2D has a valid getLastDataTime();
    while (A2D.poll() == false)
      delayMicroseconds(5);
    
    A2D.setCallback(_Callback);

    Timer.restart();
}

CTeleCounter TC_Update{TeleGroup::HARDWARE, 1};
CTelePeriod  TP_Update{TeleGroup::HARDWARE, 2};
CTeleValue   TV_maxDur(TeleGroup::HARDWARE, 3);
CRunningAverage<double> _raUpdateDurations;

 bool setTimer = false; 

  static double STATE_DURATION = CFG::STATE_DURATION_uS / 1'000'000.0; // convert to seconds

bool Hardware::canUpdate() {

  return (Timer.getStateTime() + A2D.getPollDuration() < STATE_DURATION);
}

double lastMark = 0.0;
void Hardware::update() {

  TP_Update.measure();
  TC_Update.increment();
  TV_maxDur.set(_raUpdateDurations.getAverage() * 1'000'000.0); // in microseconds


  if (A2D.poll() == false) { yield(); return; }

  if (setTimer == false) { // if we have a new A2D reading and haven't set the timer for this update cycle
      lastMark = Timer.getConnectTime();
      Timer.HW.reset();
      setTimer = true;
  }

  double stateTime = Timer.getStateTime();

  bool tryHWupdate = (stateTime + A2D.getPollDuration() < STATE_DURATION);

  if (stateTime < 0.5 * STATE_DURATION || tryHWupdate) { 

    double updateStart = Timer.getStateTime();
        getHWforState().update();  // update digital pots based on current state
    double updateEnd = Timer.getStateTime();

    _raUpdateDurations.add(updateEnd - updateStart);

  }

  setTimer = false; // reset for next cycle

}