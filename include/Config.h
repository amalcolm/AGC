#pragma once
#include "DataTypes.h"
#ifndef BUILD_STR
#define BUILD_STR "dev"
#endif



class CFG {
public:
    inline static constexpr bool TESTMODE = false;  // if true, uses polled A2D mode and _Callback by default

    inline static constexpr uint32_t LoopPeriod_mS = 3;  // 3ms

    inline static constexpr uint32_t SAMPLING_SPEED = 2'000; // A2D sampling speed in samples per second
    inline static constexpr uint32_t READING_SPEED  =   900; // A2D reading speed (per second) in triggered mode

    inline static constexpr uint32_t MAX_BLOCKSIZE   = 164;  // max number of DataType entries in a BlockType

    
    inline static constexpr const char DEVICE_VERSION[] = "0.6.0+" BUILD_STR;
    inline static constexpr const char DEVICE_NAME[]    = "AGC";
    inline static                 char HOST_VERSION[16] = "[unknown]";

};