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

  auto& [state, offsetPot1, offsetPot2, gainPot] = getPerStateHW();

  offsetPot1.update();
  offsetPot2.update();
    gainPot .update(); 


}


SPISettings    Hardware::SPIsettings(4800000, MSBFIRST, SPI_MODE1);