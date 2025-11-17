#include "DataTypes.h"
#include "Arduino.h"

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

void DataType::writeSerial() {
  Serial.write((uint8_t*)&state        , sizeof(state      ));
  Serial.write((uint8_t*)&timeStamp    , sizeof(timeStamp  ));
  Serial.write((uint8_t*)&hardwareState, sizeof(hardwareState));
  Serial.write((uint8_t*)&channels[0]  , CHANNELS_BYTESIZE  );
}




BlockType::BlockType() : timeStamp(0), state(DIRTY), count(0), data() {
  // data.fill(DataType());
}

void BlockType::writeSerial() {
  Serial.write((uint8_t*)&timeStamp, sizeof(timeStamp));
  Serial.write((uint8_t*)&state    , sizeof(state));
  Serial.write((uint8_t*)&count    , sizeof(count));

  for (uint32_t i = 0; i < count; i++)
    data.at(i).writeSerial();
  
}

void BlockType::debugSerial() {
  Serial.print("N:"); Serial.print(count);
  for(uint32_t i = 0; i < DEBUG_BLOCKSIZE && i < count; i++) {
    Serial.print("\t C"); Serial.print(i); Serial.print(":"); Serial.print(data.at(i).channels[0]);
  }
  Serial.println();
}

