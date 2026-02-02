#pragma once
#include <cstdint>
#include <array>
#include "Config.h"
#include "CEvents.h"

using StateType = uint32_t;

static constexpr uint32_t NUM_CHANNELS = 8;

static constexpr StateType DIRTY = 0xFFFFFFFF;
static constexpr StateType UNSET = 0x80000000;

struct DataType {
  static constexpr uint32_t FRAME_START = 0xEDD1FAB4;
  static constexpr uint32_t FRAME_END   = 0xEDD2FAB4;


  StateType  state;          // state of the head during this reading
  double     timestamp;      // timestamp in seconds since connection
  double     stateTime;      // time in seconds since last state change
  uint32_t   hardwareState;  //  (count & 0xFF) << 24 | offset1 << 16 | offset2 << 8 | gain      // needs 4 byte alignment
  uint32_t   sensorState;    //  preGain << 16 | postGain 
  uint32_t   channels[NUM_CHANNELS];

  DataType();
  DataType(StateType state);

  void writeSerial(bool includeFrameMarkers = true);
  void debugSerial();
};

struct BlockType {
  static constexpr uint32_t DEBUG_BLOCKSIZE = 16;
  static constexpr uint32_t FRAME_START = 0xEDB1FAB4;
  static constexpr uint32_t FRAME_END   = 0xEDB2FAB4;
  

  double   timestamp;
  uint32_t state;
  uint32_t count;
  DataType data[CFG::MAX_BLOCKSIZE];

  uint32_t numEvents;
  EventType events[CFG::MAX_EVENTS_PER_BLOCK];


  BlockType();

  void clear();

  inline bool tryAdd(const DataType& item) { if (count >= CFG::MAX_BLOCKSIZE) return false;
    data[count++] = item;
    return true;
  }

  bool tryAddEvent(const EventKind kind, double time = -1.0);

  void writeSerial(bool includeFrameMarkers = true);
  void debugSerial();
};



typedef void (*CallbackType)(BlockType*);

