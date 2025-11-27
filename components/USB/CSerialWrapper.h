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


    CSerialWrapper();
    virtual ~CSerialWrapper() = default;

    virtual void begin();
    virtual void tick();

    inline ModeType getMode() { return m_Mode; }
           ModeType setMode(ModeType mode);

    void printf(const char *pFMT, ...);
    
  private:
    ModeType m_Mode = ModeType::UNSET;

  protected:
    bool m_handshakeComplete = false;

    void put(uint8_t* pData, unsigned int dataLen);
    void put(Frame frame);

      
};
