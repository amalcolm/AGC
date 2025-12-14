#pragma once
#include <DataTypes.h>
#include <CTelemetry.h>

struct PerStateHW& getPerStateHW(DataType& data);
struct PerStateHW& getPerStateHW(BlockType* block = nullptr);

#define ERROR(fmt, ...) error_impl(__FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)


[[noreturn]] void error_impl(const char* file, int line, const char* func,
                             const char* fmt, ...);

inline uint32_t TeleCount[256]{};
inline uint32_t getCounter(uint8_t n) { uint32_t num = TeleCount[n]; TeleCount[n] = 0; return num; }


inline void Tele(TeleGroup group, int ID, float value) {
    CTelemetry::log(group, ID, value);
}

inline void Tele(TeleGroup group, uint8_t subGroup, int ID, float value) {
    CTelemetry::log(group, subGroup, ID, value);
}