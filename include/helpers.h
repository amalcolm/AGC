#pragma once
#include <DataTypes.h>

const char * getSketch();

struct PerStateHW& getPerStateHW(DataType& data);
struct PerStateHW& getPerStateHW(BlockType* block = nullptr);

#define ERROR(fmt, ...) error_impl(__FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)


[[noreturn]] void error_impl(const char* file, int line, const char* func,
                             const char* fmt, ...);


