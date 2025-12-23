#pragma once
#include <SPI.h>
#include "DataTypes.h"

class CA2D {
  public:
    enum ModeType { UNSET, CONTINUOUS, TRIGGERED };

    enum TeleKind { COUNT = 0, TIME = 1, VOLTAGE = 2, RAW = 3 };

    inline static uint32_t SAMPLING_SPEED = 8'000; // default A2D sampling speed in samples per second (only used in continuous mode)

    enum ReadState { IDLE, IGNORE, PREPARE, READ };

  public:
    CA2D(ModeType mode);
    CA2D(CallbackType callback);
    
    CA2D&     begin();
    void      setCallback(CallbackType callback) { m_fnCallback = callback; }
    void      makeCallback(BlockType* pBlock) { if (m_fnCallback) m_fnCallback(pBlock); }

    bool      readFrame(uint8_t (&raw)[32]);
    void      dataFromFrame(uint8_t (&raw)[32], DataType& data);

    // Triggered
    DataType  getData();

    // Continuous
    void      setBlockState(StateType state);
    void      prepareForRead() { m_ReadState = ReadState::PREPARE; };
    void      startRead() { m_ReadState = ReadState::READ; };
    inline bool IsDMAActive() const  { return s_dmaActive; }
    inline void pauseRead () { setRead(false); }
    inline void resumeRead() { setRead(true); }


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
    ReadState           m_ReadState  = ReadState::IDLE;

    // Continuous

    static void ISR_Data();
    bool        pollData();
    
 // DMA / continuous mode support
   static EventResponder s_spiEvent;
    static void onSpiDmaComplete(EventResponderRef);
    static volatile bool s_dmaActive;        // true while DMA SPI in progress
    static volatile bool s_dataArrived;      // true when DMA frame finished
  
    void setRead(bool enable);
    void setDebugData(DataType& data);

    volatile bool       m_dataReady = false;
    BlockType           m_BlockA;
    BlockType           m_BlockB;

    BlockType* volatile m_pBlockToFill;
    BlockType* volatile m_pBlockToSend;


    void SPIwrite(std::initializer_list<uint8_t> data);

    public:
    
    inline bool poll() { if (m_Mode != ModeType::CONTINUOUS) return false; else return pollData(); }
}; 