#pragma once
#include <cstdint>


class CFG {
public:
    inline static constexpr bool TESTMODE = false;  // if true, uses polled A2D mode and _Callback by default


    inline static constexpr uint32_t STATE_DURATION_uS     =  1'140;  // time for each state. loop will be slightly longer than this

    inline static constexpr uint32_t HEAD_SETTLE_TIME_uS   =    440;  // delay between Head change and first A2D read
    
    inline static constexpr uint32_t POT_UPDATE_PERIOD_uS  =  1'111;  // Potentiometer update rate (900 Hz or 1/A2D_READING_SPEED_Hz)
    inline static constexpr uint32_t POT_UPDATE_OFFSET_uS  =    667;  // A2D -> Potentiometer update offset, minimizes interference

    inline static constexpr uint32_t A2D_SAMPLING_SPEED_Hz =  2'000;  // A2D sampling speed 
    inline static constexpr uint32_t A2D_READING_SPEED_Hz  =    900;  // A2D reading speed when in triggered mode
 
    inline static constexpr uint32_t MAX_BLOCKSIZE         =    164;  // max number of DataType entries in a BlockType

    
    inline static constexpr char DEVICE_VERSION[]  = "0.1.2+" BUILD_STR;  // this is a #define from the build system
    inline static constexpr char DEVICE_NAME[]     = "fNIRS (Teensy 4.1)";
    inline static           char HOST_VERSION[16]  = "[unknown]";

};