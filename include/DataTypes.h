#pragma once
#include <cstdint>
#include <vector>
#include "Arduino.h"

typedef uint32_t StateType;

extern const StateType DIRTY;

struct DataType {
  static constexpr uint32_t NUM_CHANNELS = 8;
  static constexpr uint32_t CHANNELS_BYTESIZE = NUM_CHANNELS * sizeof(int);

  uint32_t   timeStamp;
  StateType  state;
  uint32_t   timeDelta;
  int        channels[NUM_CHANNELS];
  int        serialCount;

  DataType() : timeStamp(micros()), state(DIRTY), timeDelta(0), serialCount(0) { memset(&channels[0], 0, CHANNELS_BYTESIZE ); }
  DataType(StateType state) : timeStamp(micros()), state(state), timeDelta(0), serialCount(0) { memset(&channels[0], 0, CHANNELS_BYTESIZE ); }

  void debugSerial() volatile {
    Serial.print("C0:");
    Serial.println(channels[0]);
  }

  void writeSerial() volatile {
    Serial.write((uint8_t*)&timeStamp  , sizeof(timeStamp  ));
    Serial.write((uint8_t*)&state      , sizeof(state      ));
    Serial.write((uint8_t*)&timeDelta  , sizeof(timeDelta  ));
    Serial.write((uint8_t*)&channels[0], CHANNELS_BYTESIZE  );
    Serial.write((uint8_t*)&serialCount, sizeof(serialCount));
  }
};

struct BlockType {
  static constexpr uint32_t MAX_BLOCKSIZE = 4096;
  static constexpr uint32_t DEBUG_BLOCKSIZE = 16;

  uint32_t               timeStamp;
  uint32_t               state;
  uint32_t               count;
  std::vector<DataType> *data;

  BlockType() : data(new std::vector<DataType>()) { data->reserve(MAX_BLOCKSIZE); }

  ~BlockType() { if (data != NULL) { delete data; data = NULL; } }

  void debugSerial() volatile {
    Serial.print("N:"); Serial.print(data->size());
    for(uint32_t i = 0; i < DEBUG_BLOCKSIZE && i < data->size(); i++) {
      Serial.print("\t C"); Serial.print(i); Serial.print(":"); Serial.print(data->at(i).channels[0]);
    }
    Serial.println();
  }

  void writeSerial() volatile {
    if (data == NULL) return;
    
    Serial.write((uint8_t*)&timeStamp, sizeof(timeStamp));
    Serial.write((uint8_t*)&state, sizeof(state));

    uint32_t sampleCount = data->size();
    Serial.write((uint8_t*)&sampleCount, sizeof(sampleCount));

    for (DataType& item : *data) {
        item.writeSerial();
    }

    if (data->empty() == false) {
      uint8_t* pVectorData = reinterpret_cast<uint8_t*>(data->data());
      size_t totalDataSize = data->size() * sizeof(DataType);
      Serial.write(pVectorData, totalDataSize);
    }

  }

};

typedef void (*CallbackType)(volatile BlockType*);
