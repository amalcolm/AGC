
#include "DataTypes.h"
#include "Setup.h"
#include "Hardware.h"
#include "CUSB.h"

// ProcessA2D: Callback to process A2D data blocks for debugging
void _Callback(BlockType* block) {  if (!TESTMODE || block == nullptr || block->count == 0) return;

  // if we are outputting the A2D data, skip the debug below
  A2D.outputDebugBlock = false;  if (A2D.outputDebugBlock) return; 

  // get hardware for the block's state
  auto& [state, offsetPot1, offsetPot2, gainPot, tele] = getPerStateHW(block);      IGNORE(tele);  IGNORE(state);

  // get the last data point in the block
  DataType& data = block->data[block->count - 1];  

  // Output debug info to Serial

  USB.printf(     "A2D:%d"    , data.channels[0]);
  USB.printf(  "\t Sensor1:%d", offsetPot1.getSensorValue());
  USB.printf(  "\t Sensor2:%d", offsetPot2.getSensorValue());
  USB.printf(  "\t offset1:%d", offsetPot1.getLevel());
  USB.printf(  "\t offset2:%d", offsetPot2.getLevel());
  USB.printf(  "\t Gain:%d"   ,    gainPot.getLevel());
  USB.printf(  "\t Min:%d"    , offsetPot2.getRunningAverage().GetMin());
  USB.printf(  "\t Max:%d"    , offsetPot2.getRunningAverage().GetMax());
  USB.printf(  "\n"); // must end output to be parsed correctly

}





