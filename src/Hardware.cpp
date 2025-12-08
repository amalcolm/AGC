#include "Setup.h"
#include "Hardware.h"
#include "Helpers.h"
#include "CUSB.h"
#include "CAutoPot.h"
#include "CA2D.h"
#include "CHead.h"
#include "CTimer.h"
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


void Hardware::tick() {
  static double lastTime = 0.0;
  static constexpr double updateInterval = 0.01; // 100 Hz
  static bool haveData = false;

  haveData |= A2D.poll();  // main A2D polling, every cycle

  if (Timer.upTime() - lastTime < updateInterval) return;
  lastTime = Timer.upTime();

  if (haveData)
    getPerStateHW().update();  // update pots at 100Hz but only if we have new data

  haveData = false;
}


SPISettings    Hardware::SPIsettings(4800000, MSBFIRST, SPI_MODE1);