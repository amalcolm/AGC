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

SPISettings    Hardware::SPIsettings(4800000, MSBFIRST, SPI_MODE1);

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

CTimedGate gate(0.01); // 100hz
CTeleCounter TC_Update{TeleGroup::HARDWARE, 1};
CTelePeriod  TP_Update{TeleGroup::HARDWARE, 2};


void Hardware::update() {
  TP_Update.measure();
  TC_Update.increment();
  
  A2D.poll();

  if (gate.block() || A2D.IsDMAActive()) return;

  A2D.pauseRead();
  
  getPerStateHW().update();  // update pots at 100Hz but only if we have new data
  
  A2D.resumeRead();
}


