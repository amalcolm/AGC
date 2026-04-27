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

  // get hardware for the block's state
  auto& [dbg, state, Stage1, opAmp, _, __] = *getHWforState(block);

  // get the last data point in the block
  DataType& data = block->data[block->count - 1];  

  // Output debug info to Serial

  USB.printf(     "A2D:%d"       , data.channels[0]);
  USB.printf(  "\t Stage1:%d"       , Stage1.mid.lastSensorValue());
  USB.printf(  "\t OpAmp:%d"     , opAmp.lastSensorValue());
  USB.printf(  "\t Stage1_TOP:%d"   , Stage1.top.getLevel());
  USB.printf(  "\t Stage1_BOT:%d"   , Stage1.bot.getLevel());
  USB.printf(  "\t Stage1_MID:%d"   , Stage1.mid.getLevel());
  USB.printf(  "\t offset2:%d"   , opAmp.offsetPot.getLevel());
  USB.printf(  "\t Gain:%d"      , opAmp.gainPot.getLevel());
  USB.printf(  "\t Min:%d"       , opAmp.offsetPot.getRunningAverage().getMin());
  USB.printf(  "\t Max:%d"       , opAmp.offsetPot.getRunningAverage().getMax());

  USB.printf(  "\n"); // must end output with '\n' to be parsed correctly

}


