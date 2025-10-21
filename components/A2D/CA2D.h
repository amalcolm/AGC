#pragma once
#include <SPI.h>
#include <vector>
#include "CHead.h"
class CA2D {
  public:
    enum ModeType { UNSET, CONTINUOUS, TRIGGERED };

    #include "CA2D_DataTypes.h"

  public:
    CA2D(ModeType mode);
    CA2D(CA2D::CallbackType callback);
    
    CA2D&     init();
    void      setCallback(CA2D::CallbackType callback) { m_fnCallback = callback; }

    ModeType  getMode() { return m_Mode; }
    bool      readFrame(uint8_t (&raw)[27]);
    void      dataFromFrame(uint8_t (&raw)[27], CA2D::DataType& data);

    // Triggered
    CA2D::DataType getData();

    // Continuous
    void        poll();
    
    void        setBlockState(CHead::StateType state);

    inline BlockType* getBlockToSend()     { return m_pBlockToSend;         }
    inline void       releaseBlockToSend() { m_pBlockToSend->data->clear(); }
    volatile bool     isBlockReadyToSend = false;

  private:
    void      setMode(CA2D::ModeType mode);
    void      setMode_Continuous();
    void      setMode_Triggered();

    CA2D::DataType readData();

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