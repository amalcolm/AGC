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

  public:
    CA2D();
  
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
    inline bool isBusy() const  { return s_dmaActive || m_dataReady; }
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

  public:
    ModeType            m_Mode       = ModeType::UNSET;
    CallbackType        m_fnCallback = NULL;
    ReadState           m_ReadState  = ReadState::IDLE;

    // Triggered
    static void ISR_Mark();
    bool        poll_Triggered();
    uint64_t    m_dataTime = 0;
  
    // Continuous

    static void ISR_Data();
    bool        poll_Continuous();
    
 // DMA / continuous mode support
   static EventResponder s_spiEvent;
    static void onSpiDmaComplete(EventResponderRef);
    static volatile bool s_dmaActive;        // true while DMA SPI in progress
  
    void setRead(bool enable);
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
    
    inline bool poll() { return ((m_Mode == ModeType::CONTINUOUS) ? poll_Continuous() : poll_Triggered()); }
}; 