#include "DataTypes.h"
#include "Arduino.h"

static constexpr uint32_t CHANNELS_BYTESIZE = NUM_CHANNELS * sizeof(int);


DataType::DataType() 
  : state(DIRTY), timeStamp(micros()), hardwareState(0) { 
  
  memset(&channels[0], 0, CHANNELS_BYTESIZE );
}

DataType::DataType(StateType state) 
  : state(state), timeStamp(micros()), hardwareState(0) {
  
  memset(&channels[0], 0, CHANNELS_BYTESIZE ); 
}

void DataType::debugSerial() {
  Serial.print("C0:");
  Serial.println(channels[0]);
}

void DataType::writeSerial(bool includeFrameMarkers) {
  if (includeFrameMarkers) Serial.write(frameStart);
  
  Serial.write(state);
  Serial.write(timeStamp);
  Serial.write(hardwareState);
  Serial.write((uint8_t*)&channels[0]  , CHANNELS_BYTESIZE    );

  if (includeFrameMarkers) Serial.write(frameEnd);
}




BlockType::BlockType() : timeStamp(0), state(DIRTY), count(0), data() {
  // data.fill(DataType());
}

void BlockType::writeSerial(bool includeFrameMarkers) {
  if (includeFrameMarkers) Serial.write(frameStart);

  Serial.write((uint8_t*)&timeStamp, sizeof(timeStamp));
  Serial.write((uint8_t*)&state    , sizeof(state));
  Serial.write((uint8_t*)&count    , sizeof(count));

  for (uint32_t i = 0; i < count; i++)
    data[i].writeSerial(false);
  
  if (includeFrameMarkers) Serial.write(frameEnd);
}

void BlockType::debugSerial() {
  Serial.print("N:"); Serial.print(count);
  for(uint32_t i = 0; i < DEBUG_BLOCKSIZE && i < count; i++) {
    Serial.print("\t C"); Serial.print(i); Serial.print(":"); Serial.print(data[i].channels[0]);
  }
  Serial.println();
}

