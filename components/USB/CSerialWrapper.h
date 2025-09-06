#pragma once

#include "CA2D.h"
#include <array>

class CSerialWrapper {
  public:
    enum ModeType { UNSET = 0, INITIALISING = 1, TEXT = 2, RAWDATA = 4, BLOCKDATA = 8 };
    static const uint FRAMING_MASK = ModeType::RAWDATA | ModeType::BLOCKDATA;
    static const uint PRINTF_BUFFER_SIZE = 256;
    static const uint FRAMING_SIZE = 4;

    CSerialWrapper();
    virtual ~CSerialWrapper() = default;

    virtual void init();
    bool isInitialising();

    inline ModeType getMode() { return m_Mode; }
           ModeType setMode(ModeType mode);

    void printf(const char *pFMT, ...);
    void buffer(CA2D::DataType data);
    
  private:
    ModeType m_Mode = ModeType::UNSET;

    void begin();

  protected:
    bool m_handshakeComplete = false;

    void put(uint8_t* pData, uint dataLen);

    void writeRawData(CA2D::DataType* pData);
    void writeRawData(volatile CA2D::BlockType* pBlock);


    std::array<uint8_t, FRAMING_SIZE> m_BlockData_Start = { 0x0A, 0x50, 0x4B, 0x2B };
    std::array<uint8_t, FRAMING_SIZE> m_BlockData_End   = { 0x0A, 0x50, 0x4B, 0x2D };
    std::array<uint8_t, FRAMING_SIZE> m_RawData_Start   = { 0x0A, 0x52, 0x44, 0x2B };
    std::array<uint8_t, FRAMING_SIZE> m_RawData_End     = { 0x0A, 0x52, 0x44, 0x2D };
};
