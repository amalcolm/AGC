#pragma once
#include <cstdint>


class CFG {
public:
    // debugging option
    inline static constexpr bool TESTMODE = false;  // if true, call the _Callback(BlockType) by default, for debugging

    // hardware timing constants (in microseconds / hertz)
    inline static constexpr double STATE_DURATION_uS       =  3'000;  // time for each state. loop will be slightly longer than this

    inline static constexpr double HEAD_SETTLE_TIME_uS     =     60;  // delay between Head change and first A2D read
    
    inline static constexpr double POT_UPDATE_OFFSET_uS    =      0;  // A2D -> Potentiometer update offset, minimizes interference

    
    // A2D configuration
    inline static constexpr bool   A2D_USE_CONTINUOUS_MODE =  false;  // use continuous A2D mode; else triggered mode with interrupts
    inline static constexpr double A2D_SAMPLING_SPEED_Hz   =  8'000;  // A2D sampling speed set in CONFIG1 register

    inline static constexpr double A2D_READING_PERIOD_uS   =    300;  // A2D reading speed. Can differ from the CONFIG1 sampling speed
 
    inline static constexpr double A2D_QUIET_PERIOD_uS     =      0;  // experimental quiet time befor reading


    // program constants
    inline static constexpr uint32_t MAX_BLOCKSIZE           =    164;  // max number of DataType entries in a BlockType
    inline static constexpr uint32_t MAX_EVENTS_PER_BLOCK    =    512;  // max number of EventType entries in a BlockType

    
    inline static constexpr char DEVICE_VERSION[]  = "0.1.4+" BUILD_STR;  // this is a #define from the build system
    inline static constexpr char DEVICE_NAME[]     = "fNIRS (Teensy 4.1)";
    inline static           char HOST_VERSION[16]  = "[unknown]";

};


/* @2Khz A2D; min state_duration is ~1130uS (with normal settings below)
 *   HeadSettle: 440uS, PotUpdate: every 1111uS with 667uS offset
 *   state_duration of 3050 gives three samples per state
 */
