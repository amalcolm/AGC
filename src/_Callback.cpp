#include "DataTypes.h"
#include "Setup.h"
#include "Hardware.h"
#include "CUSB.h"
#include "Config.h"

// Callback to process A2D data blocks for debugging
void _Callback(BlockType* block) {  if (!CFG::TESTMODE || block == nullptr || block->count == 0) return;

  // Choose whether to output the A2D data.
  A2D.outputDebugBlock = false; 
  if (A2D.outputDebugBlock) return;   // If so, skip rest of function

  // get hardware for the block's state                                                                 // avoid warnings on unused variables
  auto& [state, offsetPot1, offsetPot2, gainPot] = getHWforState(block);                                IGNORE(state);

  // get the last data point in the block
  DataType& data = block->data[block->count - 1];  

  // Output debug info to Serial

  USB.printf(     "A2D:%d"    , data.channels[0]);
  USB.printf(  "\t Sensor1:%d", offsetPot1.lastSensorValue());
  USB.printf(  "\t Sensor2:%d", offsetPot2.lastSensorValue());
  USB.printf(  "\t offset1:%d", offsetPot1.getLevel());
  USB.printf(  "\t offset2:%d", offsetPot2.getLevel());
  USB.printf(  "\t Gain:%d"   ,    gainPot.getLevel());
  USB.printf(  "\t Min:%d"    , offsetPot2.getRunningAverage().getMin());
  USB.printf(  "\t Max:%d"    , offsetPot2.getRunningAverage().getMax());

  USB.printf(  "\n"); // must end output with '\n' to be parsed correctly

}





