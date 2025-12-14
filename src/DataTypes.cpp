#include "DataTypes.h"
#include "CTimer.h"
#include "Setup.h"
#include "Arduino.h"
#include "CUSB.h"
#include "CHead.h"

static constexpr uint32_t CHANNELS_BYTESIZE = NUM_CHANNELS * sizeof(int);


DataType::DataType() 
  : state(DIRTY), hardwareState(0) { 
  timeStamp = Timer.getConnectTime();
  memset(&channels[0], 0, CHANNELS_BYTESIZE );
}

DataType::DataType(StateType state) 
  : state(state), hardwareState(0) {

  timeStamp = Timer.getConnectTime();
  memset(&channels[0], 0, CHANNELS_BYTESIZE ); 
}

void DataType::debugSerial() {
  USB.printf("C0:%d\n", channels[0]);
}

void DataType::writeSerial(bool includeFrameMarkers) {
  if (includeFrameMarkers) USB.write(frameStart);
  
  USB.write(state);
  USB.write(timeStamp);
  USB.write(hardwareState);
  USB.write(sensorState);
  USB.write((uint8_t*)&channels[0], CHANNELS_BYTESIZE);

  if (includeFrameMarkers) USB.write(frameEnd);
}




BlockType::BlockType() : timeStamp(0.0), state(DIRTY), count(0), data() {
  for (uint32_t i = 0; i < MAX_BLOCKSIZE; i++) {
    data[i] = DataType();
  }

  if (Ready && TESTMODE && A2D.getMode() == CA2D::ModeType::TRIGGERED) {
    data[0] = A2D.getData();
    timeStamp = Timer.getConnectTime();
    count = 1;
  }

}
void BlockType::writeSerial(bool includeFrameMarkers) {
  if (includeFrameMarkers) USB.write(frameStart);

  USB.write(state);
  USB.write(timeStamp);
  USB.write(count);

  for (uint32_t i = 0; i < count; i++) {
    DataType& item = data[i];

    // omit state as all data in block share same state
    USB.write(item.timeStamp);
    USB.write(item.hardwareState);
    USB.write(item.sensorState);
    USB.write((uint8_t*)&item.channels[0], CHANNELS_BYTESIZE);
  }
    
  if (includeFrameMarkers) USB.write(frameEnd);
}

void BlockType::debugSerial() {
  USB.printf("N:%d", count);
  for(uint32_t i = 0; i < DEBUG_BLOCKSIZE && i < count; i++) {
    USB.printf("\t C%d:%d", i, data[i].channels[0]);
  }
  USB.printf("\n");
}

