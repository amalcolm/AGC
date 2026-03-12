#include "Setup.h"
#include "Hardware.h"
#include "CMasterTimer.h"
#include "CHead.h"
#include "CUSB.h"
#include "Config.h"
#include "Helpers.h"
#include "CTelemetry.h"


BlockType dbgBlock1;
BlockType dbgBlock2;

void _setup() {
  activityLED.set();

  Hardware::begin();                                         //  int n = 0; while (1) { n = (n + 1) % 3; for (int i = 0; i < 3; i++) digitalWrite(i+37, n==i ? LOW : HIGH); delay(500);}

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

  auto [state, TIA, offsetPot2, gainPot, _] = getHWforState(Head.RED1);

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

      TIA.update();

      if (TIA.inZone) offsetPot2.update();
//    if (offsetPot2.inZone)   gainPot .update();


    TT_pots.stop();

    static uint8_t seq = 0;

    DataType d(state);
    d.stateTime = Timer.getStateTime();
    d.hardwareState =
      (uint64_t(TIA.mid.getLevel() & 0xFFu) << 56) |
      (uint64_t(TIA.top.getLevel() & 0xFFu) << 48) |
      (uint64_t(TIA.bot.getLevel() & 0xFFu) << 40) |
      (uint64_t(++seq)                         << 32) |
      (uint64_t(offsetPot2.getLevel() & 0xFFu) << 24) |
      (uint64_t(gainPot   .getLevel() & 0xFFu) << 16) | 
      (0xFFFFu);

    d.sensorState = (uint32_t(tia_value) << 16) | uint32_t(opamp_val);


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
