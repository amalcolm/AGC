#include "Setup.h"
#include "Hardware.h"
#include "Helpers.h"
#include "CUSB.h"
#include "CAutoPot.h"
#include "CA2D.h"
#include "CHead.h"
#include "CTimer.h"
#include "CTimedGate.h"
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

CTimedGate gate(0.005);    // 200Hz

void Hardware::update() {

  TeleCount[1]++;

  A2D.poll();  // main A2D polling, every cycle

 if (gate.notDue()) return;  // update hardware at 200Hz

  TeleCount[5]++;

  getPerStateHW().update();  // update pots at 200Hz but only if we have new data
//  delayMicroseconds(40); // small delay to allow pot settling
}


