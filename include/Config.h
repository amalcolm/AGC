#pragma once
#include <cstdint>


class CFG {
public:
    inline static constexpr bool TESTMODE = false;  // if true, uses polled A2D mode and _Callback by default

    inline static constexpr uint32_t LoopPeriod_uS       =  8'000;  // 8ms

    inline static constexpr uint32_t HEAD_SETTLE_TIME_uS =   550; // microseconds for head to settle after state change

    inline static constexpr uint32_t SAMPLING_SPEED      =  2'000; // A2D sampling speed in samples per second
    inline static constexpr uint32_t READING_SPEED       =    900; // A2D reading speed (per second) in triggered mode
    inline static constexpr uint32_t READ_DELAY_uS       =    500; // microseconds to wait after A2D read before next action

    inline static constexpr uint32_t MAX_BLOCKSIZE      = 164;  // max number of DataType entries in a BlockType

    
    inline static constexpr const char DEVICE_VERSION[] = "0.6.0+" BUILD_STR;  // this is a #define from the build system
    inline static constexpr const char DEVICE_NAME[]    = "AGC";
    inline static                 char HOST_VERSION[16] = "[unknown]";

};