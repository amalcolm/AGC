#include "Temp\Temp.h"

namespace Temp {

constexpr uint32_t LOOP_MS    = 10;  // approx. loop time

constexpr uint8_t  CS_A2D     = 20;  // chip selects
constexpr uint8_t  CS_OFFSET1 = 23;
constexpr uint8_t  CS_OFFSET2 = 21;
constexpr uint8_t  CS_GAIN    = 22;

constexpr uint8_t  SP_GAIN        = 14;  // Sensor pins
constexpr uint8_t  SP_SELECTIVITY = 15;

bool InZone = false;

int Offset1 = 127;  uint8_t Offset1_SP = SP_SELECTIVITY;  int Offset1_SP_value = 0;
int Offset2 = 127;  uint8_t Offset2_SP = SP_GAIN;         int Offset2_SP_value = 0;
int Gain    =   1;  uint8_t    Gain_SP = SP_GAIN;         int    Gain_SP_value = 0;

const SPISettings SPISETTINGS(1600000, MSBFIRST, SPI_MODE1);
constexpr uint8_t ACTIVITY_LED = 4;

}


#include "Temp\Utilities.h"
#include "Temp\Output.h"
#include "Temp\A2D.h"
#include "Temp\Offset1.h"
#include "Temp\Offset2.h"
#include "Temp\Gain.h"


#include "Temp\Setup.h"


namespace Temp {

void loop() {
  digitalWrite(ACTIVITY_LED, !digitalRead(ACTIVITY_LED));  // show status - solid blue is in setup, flickering is run

  updateOffset1();
//  updateOffset2();
//  updateGain();

  int splitWait = 3000; 
  delayMicroseconds(splitWait/2);
  int val = readTheLEDData(); 
  delayMicroseconds(splitWait/2);

  Output(val);
  




  delay(LOOP_MS);
}


}