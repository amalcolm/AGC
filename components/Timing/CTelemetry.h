#pragma once
#include <cstdint>
#include <vector>

using Frame = uint32_t;

class CTelemetry {
public:
    static constexpr size_t maxCapacity = 256;
    static constexpr size_t initialCapacity = 16;

    static std::vector<CTelemetry*> pool;
    static CTelemetry* Rent();
    static void Return(CTelemetry* item);

public:
    enum class Group : uint8_t {
        NONE     = 0x00,
        PROGRAM  = 0x01,

        A2D      = 0x11,
        HARDWARE = 0x12,
        USB      = 0x13,
        HEAD     = 0x14,

        UNSET    = 0xFF
    };

    double   timeStamp;
    
    Group    group;
    uint8_t  subGroup;
    uint16_t ID;

    float    value;

    void reset();
    void writeSerial(bool includeFrameMarkers = true);

    static void init();

    static void log(Group group,                   uint16_t ID, float value);
    static void log(Group group, uint8_t subGroup, uint16_t ID, float value);

  	static constexpr Frame frameStart = 0xED71FAB4;  // 71/72 = Telemetry Packet
    static constexpr Frame frameEnd   = 0xED72FAB4;



};