#pragma once
#include <SPI.h>
#include "DataTypes.h"

class CA2D {
  public:
    enum ModeType { UNSET, CONTINUOUS, TRIGGERED };

    enum TeleKind { TICK = 0, TIME = 1, VOLTAGE = 2, RAW = 3 };

  public:
    CA2D(ModeType mode);
    CA2D(CallbackType callback);
    
    CA2D&     begin();
    void      setCallback(CallbackType callback) { m_fnCallback = callback; }
    void      makeCallback(BlockType* pBlock) { if (m_fnCallback) m_fnCallback(pBlock); }

    bool      readFrame(uint8_t (&raw)[27]);
    void      dataFromFrame(uint8_t (&raw)[27], DataType& data);

    // Triggered
    DataType  getData();

    // Continuous
    bool      poll();
    
    void      setBlockState(StateType state);

    inline ModeType   getMode() { return m_Mode; }
    volatile bool     outputDebugBlock = true;

  private:
    void      setMode(ModeType mode);
    void      setMode_Continuous();
    void      setMode_Triggered();

    DataType readData();

    int m_pinDataReady{9};

  private:
    ModeType            m_Mode       = ModeType::UNSET;
    CallbackType        m_fnCallback = NULL;

    // Continuous

    static void ISR_Data();
    bool        pollData();


    volatile bool       m_dataReady = false;
    BlockType           m_BlockA;
    BlockType           m_BlockB;

    BlockType* volatile m_pBlockToFill;
    BlockType* volatile m_pBlockToSend;

    

    void SPIwrite(std::initializer_list<uint8_t> data);
}; 