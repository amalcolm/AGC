#pragma once
#include <SPI.h>
#include "DataTypes.h"

class CA2D {
  public:
    enum ModeType { UNSET, CONTINUOUS, TRIGGERED };

    enum TeleKind { COUNT = 0, TIME = 1, VOLTAGE = 2, RAW = 3 };

    inline static uint32_t SAMPLING_SPEED = 1000; // default A2D sampling speed in samples per second (only used in continuous mode)

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
    void      setBlockState(StateType state);
    inline void clear() {
    __disable_irq();
      m_dataReady = false;
    __enable_irq();
    }


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

    public:
    
    inline bool poll() { if (m_Mode != ModeType::CONTINUOUS) return false; else return pollData(); }
}; 