#pragma once
#include <DataTypes.h>

const char * getSketch();

struct PerStateHW& getPerStateHW(DataType& data);
struct PerStateHW& getPerStateHW(BlockType* block = nullptr);


// 24-bit sign-extend (two’s-complement)
inline int32_t be24_to_s32(const uint8_t b2, const uint8_t b1, const uint8_t b0) {
  int32_t v = (int32_t(b2) << 16) | (int32_t(b1) << 8) | int32_t(b0);
  if (v & 0x00800000) v |= 0xFF000000; // sign-extend bit 23
  return v;
}

void error(const char *msg, ...);


