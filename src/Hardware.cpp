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

  Timer.restart();  // only done once
}

CTeleCounter TC_Update{TeleGroup::HARDWARE, 1};
CTelePeriod  TP_Update{TeleGroup::HARDWARE, 2};

void Hardware::update() {
  TP_Update.measure();
  TC_Update.increment();

  getPerStateHW().update();  // update pots
  
  A2D.getAndAdd();

  Timer.Delay_uS(CFG::READ_DELAY_uS);

//  if (gate.block()) return;  // limit update rate to gate frequency

}


