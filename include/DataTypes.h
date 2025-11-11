#pragma once
#include <cstdint>
#include <array>

typedef uint32_t StateType;

extern const StateType DIRTY;

struct DataType {
  static constexpr uint32_t NUM_CHANNELS = 8;
  static constexpr uint32_t CHANNELS_BYTESIZE = NUM_CHANNELS * sizeof(int);

  StateType  state;          // state of the head during this reading
  uint32_t   timeStamp;
  uint32_t   hardwareState;  //  (count & 0xFF) << 24 | offset1 << 24 | offset2 << 16 | gain << 8      // needs 4 byte alignment
  uint32_t   channels[NUM_CHANNELS];

  DataType();
  DataType(StateType state);

  void debugSerial();
  void writeSerial();
};

struct BlockType {
  static constexpr uint32_t MAX_BLOCKSIZE   = 64;
  static constexpr uint32_t DEBUG_BLOCKSIZE = 16;

  uint32_t timeStamp;
  uint32_t state;
  uint32_t count;
  std::array<DataType, MAX_BLOCKSIZE> data;

  BlockType();

  inline void clear() {
    count = 0;
    state = DIRTY;
  }

  inline bool push_back(const DataType& item) { if (count >= MAX_BLOCKSIZE) return false;
    data[count++] = item;
    return true;
  }

  void writeSerial();
  void debugSerial();
};

typedef void (*CallbackType)(BlockType*);
