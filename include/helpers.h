#pragma once
#include <DataTypes.h>
#include <CTelemetry.h>

struct PerStateHW& getPerStateHW(DataType& data);
struct PerStateHW& getPerStateHW(BlockType* block = nullptr);

#define ERROR(fmt, ...) error_impl(__FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)


[[noreturn]] void error_impl(const char* file, int line, const char* func,
                             const char* fmt, ...);


inline void Tele(CTelemetry::Group group, int ID, float value) {
    CTelemetry::log(group, ID, value);
}

inline void Tele(CTelemetry::Group group, uint8_t subGroup, int ID, float value) {
    CTelemetry::log(group, subGroup, ID, value);
}