#pragma once
#include <cstdint>


class CFG {
public:
    inline static constexpr bool TESTMODE = false;  // if true, uses polled A2D mode and _Callback by default


    inline static constexpr uint32_t LOOP_PERIOD_uS      = 12'000;  // 10ms main loop period

    inline static constexpr uint32_t HEAD_SETTLE_TIME_uS =    550;  // delay between Head change and first A2D read

    inline static constexpr uint32_t SAMPLING_SPEED_Hz   =  2'000;  // A2D sampling speed 

    inline static constexpr uint32_t READING_SPEED_Hz    =    900;  // A2D reading speed when in triggered mode
 
    inline static constexpr uint32_t MAX_BLOCKSIZE       =    164;  // max number of DataType entries in a BlockType

    
    inline static constexpr char DEVICE_VERSION[]  = "0.6.0+" BUILD_STR;  // this is a #define from the build system
    inline static constexpr char DEVICE_NAME[]     = "AGC";
    inline static           char HOST_VERSION[16]  = "[unknown]";

};