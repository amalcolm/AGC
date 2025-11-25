#pragma once
#include <cstdint>
#include <array>

using StateType = uint32_t;
using Frame = uint32_t;

extern const StateType DIRTY;
static constexpr uint32_t NUM_CHANNELS = 8;

struct DataType {

  StateType  state;          // state of the head during this reading
  uint32_t   timeStamp;
  uint32_t   hardwareState;  //  (count & 0xFF) << 24 | offset1 << 24 | offset2 << 16 | gain << 8      // needs 4 byte alignment
  uint32_t   channels[NUM_CHANNELS];

  Frame frameStart = 0x8A52442B;
  Frame frameEnd   = 0x8A52442D;


  DataType();
  DataType(StateType state);

  void writeSerial(bool includeFrameMarkers = true);
  void debugSerial();
};

struct BlockType {
  static constexpr uint32_t MAX_BLOCKSIZE   = 64;
  static constexpr uint32_t DEBUG_BLOCKSIZE = 16;

  uint32_t timeStamp;
  uint32_t state;
  uint32_t count;
  DataType data[MAX_BLOCKSIZE];

  Frame frameStart = 0x8A514B2B;
  Frame frameEnd   = 0x8A514B2D;

  BlockType();

  inline void clear() {
    count = 0;
    state = DIRTY;
  }

  inline bool push_back(const DataType& item) { if (count >= MAX_BLOCKSIZE) return false;
    data[count++] = item;
    return true;
  }

  void writeSerial(bool includeFrameMarkers = true);
  void debugSerial();
};



typedef void (*CallbackType)(BlockType*);
