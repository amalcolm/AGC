#pragma once
#include <cstdint>
#include <vector>
#include <deque>

using Frame = uint32_t;

    enum class TeleGroup : uint8_t {
        NONE     = 0x00,
        PROGRAM  = 0x01,
        HARDWARE = 0x02,

        A2D      = 0x11,
        DIGIPOTS = 0x12,
        USB      = 0x13,
        HEAD     = 0x14,
        TIMER    = 0x15,

        UNSET    = 0xFF
    };

class CTelemetry {
public:
    static constexpr size_t maxCapacity     = 256;
    static constexpr size_t initialCapacity =  16;

    static std::vector<CTelemetry*> pool;
    static CTelemetry* Rent();
    static void Return(CTelemetry* item);

public:

    double    timeStamp;

    TeleGroup group;
    uint8_t   subGroup;
    uint16_t  ID;

    float    value;


protected:  
    CTelemetry(TeleGroup group = TeleGroup::NONE, uint8_t subGroup = 0, uint16_t ID = 0 )
        : timeStamp(0.0), group(group), subGroup(subGroup), ID(ID) {};

public:

    void reset();
    void writeSerial(bool includeFrameMarkers = true);

    static void init();

    static void log(TeleGroup group,                   uint16_t ID, float value);
    static void log(TeleGroup group, uint8_t subGroup, uint16_t ID, float value);

  	static constexpr Frame frameStart = 0xED71FAB4;  // 71/72 = Telemetry Packet
    static constexpr Frame frameEnd   = 0xED72FAB4;

    static void _registerCounter(CTelemetry* counter);
    static void logAll();

private:
    static std::deque<CTelemetry*> allTelemetries;


};

#include "1. CTeleCounter.h"
#include "2. CTeleTimer.h"


#include "CTimedGate.h" // not stricly needed here, but helps to minimize #includes