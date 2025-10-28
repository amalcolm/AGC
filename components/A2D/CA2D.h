#pragma once
#include <SPI.h>
#include "DataTypes.h"

class CA2D {
  public:
    enum ModeType { UNSET, CONTINUOUS, TRIGGERED };


  public:
    CA2D(ModeType mode);
    CA2D(CallbackType callback);
    
    CA2D&     init();
    void      setCallback(CallbackType callback) { m_fnCallback = callback; }

    ModeType  getMode() { return m_Mode; }
    bool      readFrame(uint8_t (&raw)[27]);
    void      dataFromFrame(uint8_t (&raw)[27], DataType& data);

    // Triggered
    DataType getData();

    // Continuous
    void        poll();
    
    void        setBlockState(StateType state);

    inline BlockType* getBlockToSend()     { return m_pBlockToSend;         }
    inline void       releaseBlockToSend() { m_pBlockToSend->data->clear(); }
    
  private:
    void      setMode(ModeType mode);
    void      setMode_Continuous();
    void      setMode_Triggered();

    DataType readData();

    int m_pinDataReady{9};

  private:
    static SPISettings  g_settings;

    ModeType            m_Mode       = ModeType::UNSET;
    CallbackType        m_fnCallback = NULL;

    // Continuous

    static void ISR_Data();
    void        pollData();


    volatile bool       m_dataReady = false;
    BlockType           m_BlockA;
    BlockType           m_BlockB;

    BlockType* volatile m_pBlockToFill;
    BlockType* volatile m_pBlockToSend;

    

    void SPIwrite(std::initializer_list<uint8_t> data);
}; 