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

uint32_t POT_DELAY_TICKS = CTimerBase::microsecondsToTicks(CFG::POT_UPDATE_OFFSET_uS);

static bool _setTimer = false;
static bool _haveRead = false;
static bool _haveUpdated = false;


void Hardware::resetTiming() {
  _setTimer = false;
  _haveRead = false;
  _haveUpdated = false;
}


void Hardware::update() {
  TP_Update.measure();
  TC_Update.increment();
  
  _haveRead = A2D.poll();

  if (_haveRead && !_setTimer) {
    Timer.HW.reset();
    _setTimer = true;
    _haveUpdated = false;
  }

  if (_haveUpdated) return;

  if (Timer.HW.waiting()) return;  // limit update rate to gate frequency

  getHWforState().update();  // update pots

  _setTimer = false;
  _haveUpdated = true;
}


