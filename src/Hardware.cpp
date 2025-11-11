#include "Setup.h"
#include "Hardware.h"
#include "Helpers.h"
#include "CUSB.h"
#include "CAutoPot.h"
#include "CA2D.h"
#include "CHead.h"
#include "CTimer.h"
#include <map>


void Hardware::init() {
    // Initialize all hardware components
    SPI .begin();
    USB .init();
    BUT .init();
    LED .init();
    Head.init();
    A2D .init();

    delay(100); // Allow time for hardware to stabilize

    USB.printf("CPU Frequency: %.0f Mhz\r\n", F_CPU / 1000000.0f);
    Timer.restart();
}



void Hardware::tick() {

  auto& [state, offsetPot1, offsetPot2, gainPot] = getPerStateHW();

  offsetPot1.update();
  offsetPot2.update();
  if (offsetPot1.inZone) gainPot.update(); 


}