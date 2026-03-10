#include "Setup.h"
#include "Hardware.h"
#include "CMasterTimer.h"
#include "CHead.h"
#include "CUSB.h"
#include "Config.h"
#include "Helpers.h"
#include "CTelemetry.h"

CDigiPot tia_Upper   (CS.offset1Upper);
CDigiPot tia_Lower   (CS.offset1Lower);
CDigiPot tia_pot     (CS.offset1, SP. preGain, 10);
CDigiPot opamp_offset(CS.offset2, SP.postGain, 10);
CDigiPot opamp_gain  (CS.gain,    SP.postGain, 10);

COffsetPot offsetPot1(CS.offset1, SP. preGain, 10, 100);
COffsetPot offsetPot2(CS.offset2, SP.postGain, 10, 300);
CGainPot   gainPot   (CS.gain,    SP.postGain, 10);

bool useHelpers = false; 

BlockType dbgBlock1;
BlockType dbgBlock2;

void _setup() {
  activityLED.set();

  Hardware::begin();                                         //  int n = 0; while (1) { n = (n + 1) % 3; for (int i = 0; i < 3; i++) digitalWrite(i+37, n==i ? LOW : HIGH); delay(500);}


  tia_Upper.begin(128);
  tia_Lower.begin(255);
  tia_pot.begin(255);
  opamp_offset.begin(255);
  opamp_gain.begin(1);

  Ready = true;

  dbgBlock1.clear();
  dbgBlock2.clear();
  
  activityLED.clear();
}

CTeleTimer TT_pots{TeleGroup::DIGIPOTS, 0x01};

uint32_t count = 0;
void _loop() {
  static BlockType* dbgBlock = &dbgBlock1;
  constexpr int NUM_DATA_IN_BLOCK = 8;

  LED.write(count++); // toggle LED to indicate loop is running

  delay(10);
  return;

  USB.update();
  USB.waitForHandshake();

  uint16_t tia_value;
  uint16_t opamp_val;

  Timer.state.reset();

  for (int i = 0; i < NUM_DATA_IN_BLOCK; i++) {

    TT_pots.start();

    if (useHelpers) {

      offsetPot1.update();

      if (offsetPot1.inZone) offsetPot2.update();
//    if (offsetPot2.inZone)   gainPot .update();

        tia_pot = offsetPot1.getLevel();
        tia_value = offsetPot1.lastSensorValue();

        opamp_offset = offsetPot2.getLevel();
        opamp_val = offsetPot2.lastSensorValue();
    }
    else 
    {
      tia_value = tia_pot.readAverage();
      bool tia_inZone;
      bool opamp_inZone;

      if (tia_value > 612) tia_pot.offsetLevel( 1);
      if (tia_value < 412) tia_pot.offsetLevel(-1);

      tia_inZone = tia_value > 100 && tia_value < 924;
      
      if (tia_inZone) {
        uint16_t opamp_value = opamp_offset.readAverage();

        if (opamp_value > 812) opamp_offset.offsetLevel( 1);
        if (opamp_value < 212) opamp_offset.offsetLevel(-1);

        opamp_inZone = opamp_value > 100 && opamp_value < 924;
      }

      if (opamp_inZone) {
      // ignoring gain for now 8        
      }

    }
    TT_pots.stop();

          opamp_val = opamp_offset.readAverage(20);

    static uint8_t seq = 0;

    DataType d(Head.RED1); // or DIRTY/UNSET if you prefer
    d.stateTime = Timer.getStateTime();
    d.hardwareState =
      (uint64_t(tia_pot     .getLevel() & 0xFFu) << 56) |
      (uint64_t(tia_Upper   .getLevel() & 0xFFu) << 48) |
      (uint64_t(tia_Lower   .getLevel() & 0xFFu) << 40) |
      (uint64_t(++seq)                           << 32) |
      (uint64_t(opamp_offset.getLevel() & 0xFFu) << 24) |
      (uint64_t(opamp_gain  .getLevel() & 0xFFu) << 16) | 
      (0xFFFFu);

    d.sensorState = (uint32_t(tia_value) << 16) | uint32_t(opamp_val);

    // optional: put same values in channels for easy plotting
    d.channels[0] = tia_value;
    d.channels[1] = opamp_val;

    dbgBlock->timestamp = d.timestamp;
    dbgBlock->state = d.state;
    dbgBlock->tryAdd(d);
  }
  if (dbgBlock->count > 8) {
    USB.buffer(dbgBlock);

    dbgBlock = (dbgBlock == &dbgBlock1) ? &dbgBlock2 : &dbgBlock1;
    
    dbgBlock->clear();
  }



  CTelemetry::logAll();
  

  delay(1);
  activityLED.toggle();             // Indicate activity on LED
}
