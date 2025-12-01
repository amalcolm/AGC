#include "DataTypes.h"
#include "CTimer.h"
#include "Setup.h"
#include "Arduino.h"
#include "CUSB.h"
#include "CHead.h"

static constexpr uint32_t CHANNELS_BYTESIZE = NUM_CHANNELS * sizeof(int);


DataType::DataType() 
  : state(DIRTY), hardwareState(0) { 
  timeStamp = Timer.getTimestamp();
  memset(&channels[0], 0, CHANNELS_BYTESIZE );
}

DataType::DataType(StateType state) 
  : state(state), hardwareState(0) {

  timeStamp = Timer.getTimestamp();
  memset(&channels[0], 0, CHANNELS_BYTESIZE ); 
}

void DataType::debugSerial() {
  Serial.print("C0:");
  Serial.println(channels[0]);
}

void DataType::writeSerial(bool includeFrameMarkers) {
  if (includeFrameMarkers) USB.write(frameStart);
  
  USB.write(state);
  USB.write(timeStamp);
  USB.write(hardwareState);
  USB.write((uint8_t*)&channels[0], CHANNELS_BYTESIZE);

  if (includeFrameMarkers) USB.write(frameEnd);
}




BlockType::BlockType() : timeStamp(0), state(DIRTY), count(0), data() {
  for (uint32_t i = 0; i < MAX_BLOCKSIZE; i++)
    data[i] = DataType();

  if (Ready && TESTMODE && A2D.getMode() == CA2D::ModeType::TRIGGERED) {
    data[0] = A2D.getData();
    timeStamp = Timer.getTimestamp();
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

    USB.write(item.timeStamp);
    USB.write(item.hardwareState);
    USB.write((uint8_t*)&item.channels[0], CHANNELS_BYTESIZE);
  }
    
  if (includeFrameMarkers) USB.write(frameEnd);
}

void BlockType::debugSerial() {
  Serial.print("N:"); Serial.print(count);
  for(uint32_t i = 0; i < DEBUG_BLOCKSIZE && i < count; i++) {
    Serial.print("\t C"); Serial.print(i); Serial.print(":"); Serial.print(data[i].channels[0]);
  }
  Serial.println();
}

