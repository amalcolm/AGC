#pragma once
#include <DataTypes.h>
#include <CTelemetry.h>

#define IGNORE(x) (void)(x)

// Returns the hardware struct for a given state
struct HWforState& getHWforState(DataType& data);  // must also include Hardware.h
struct HWforState& getHWforState(BlockType* block = nullptr);


// Telemetry logging helper functions
inline void Tele(TeleGroup group, int ID, float value) {
    CTelemetry::log(group, ID, value);
}

inline void Tele(TeleGroup group, uint8_t subGroup, int ID, float value) {
    CTelemetry::log(group, subGroup, ID, value);
}


// Error handling macro, printf style
#define ERROR(fmt, ...) error_impl(__FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)


[[noreturn]] void error_impl(const char* file, int line, const char* func,
                             const char* fmt, ...);


