#pragma once

#include "CA2D.h"
#include <array>

class CSerialWrapper {
  public:
    enum ModeType { UNSET = 0, INITIALISING = 1, TEXT = 2, RAWDATA = 3, BLOCKDATA = 4, placeholder = 5 };
    static const unsigned int NUM_MODETYPES = ModeType::placeholder;
    
    static const unsigned int FRAMING_MASK = ModeType::RAWDATA | ModeType::BLOCKDATA;
    static const unsigned int PRINTF_BUFFER_SIZE = 256;
    static const unsigned int FRAMING_SIZE = 4;

    using Frame = std::array<uint8_t, FRAMING_SIZE>;


    CSerialWrapper();
    virtual ~CSerialWrapper() = default;

    virtual void init();
    bool isInitialising();

    inline ModeType getMode() { return m_Mode; }
           ModeType setMode(ModeType mode);

    void printf(const char *pFMT, ...);
    
  private:
    ModeType m_Mode = ModeType::UNSET;

    void begin();

  protected:
    bool m_handshakeComplete = false;

    void put(uint8_t* pData, unsigned int dataLen);

      

    Frame m_BlockData_Start = { 0x0A, 0x50, 0x4B, 0x2B };
    Frame m_BlockData_End   = { 0x0A, 0x50, 0x4B, 0x2D };
    Frame m_RawData_Start   = { 0x0A, 0x52, 0x44, 0x2B };
    Frame m_RawData_End     = { 0x0A, 0x52, 0x44, 0x2D };
};
