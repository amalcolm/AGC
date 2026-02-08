#pragma once
#include <SPI.h>
#include "DataTypes.h"

class CA2D {
  public:
    enum ModeType { UNSET, CONTINUOUS, TRIGGERED };

    enum TeleKind { COUNT = 0, TIME = 1, VOLTAGE = 2, RAW = 3 };

    
    enum ReadState { IDLE, IGNORE, PREPARE, READ };
    SPISettings spiSettings{4'800'000, MSBFIRST, SPI_MODE1};

    static const std::array<std::pair<uint32_t, uint8_t>, 8> SpeedLookup;
    static CA2D* Singleton;

    int m_pinDataReady{9};
    volatile bool outputDebugBlock = false;

  public:
    CA2D();
  
    void      begin();
    void      setCallback(CallbackType callback) { m_fnCallback = callback; }
    void      makeCallback(BlockType* pBlock)    { if (m_fnCallback) m_fnCallback(pBlock); }

    DataType  getData();
    DataType  getData_DMA();

    // Continuous
    void      setBlockState(StateType state);
    void      prepareForRead() { m_ReadState = ReadState::PREPARE; };
    void      startRead() { m_ReadState = ReadState::READ; };

    inline ModeType   getMode() { return m_Mode; }

  private:
    void      setMode(ModeType mode);
    void      setMode_Continuous();
    void      setMode_Triggered();

    bool      readFrame(uint8_t (&raw)[32]);
    void      dataFromFrame(uint8_t (&raw)[32], DataType& data);


  public:
    ModeType            m_Mode       = ModeType::UNSET;
    CallbackType        m_fnCallback = NULL;
    ReadState           m_ReadState  = ReadState::IDLE;

    static void ISR_Data();
    
 // DMA / continuous mode supportw
    static inline EventResponder s_spiEvent{};
    static void onSpiDmaComplete(EventResponderRef);
    static inline volatile bool s_dmaActive = false;        // true while DMA SPI in progress
    
    void setDebugData(DataType& data);
    uint8_t getConfig1() const;

    volatile bool       m_dataReady = false;
    double              m_dataStateTime = 0.0;
    BlockType           m_BlockA;
    BlockType           m_BlockB;

    BlockType* volatile m_pBlockToFill;
    BlockType* volatile m_pBlockToSend;

    void SPIwrite(std::initializer_list<uint8_t> data);
 
  public:
    bool poll();
}; 

    // buffers for DMA SPI transfers - must be 32-byte aligned for cache management on Teensy 4.x
extern uint8_t m_rxBuffer[32];
extern uint8_t m_txBuffer[32];
extern uint8_t m_frBuffer[32];