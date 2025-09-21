#pragma once
#include <SPI.h>
#include "CHead.h"
#include "PinHelpers.h"
#include <vector>

class CA2D {
  public:
    enum ModeType { UNSET, CONTINUOUS, TRIGGERED };

    #include "CA2D_DataTypes.h"

  public:
    CA2D(ModeType mode);
    CA2D(CA2D::CallbackType callback);
    
    CA2D*     init();
    void      setCallback(CA2D::CallbackType callback) { m_fnCallback = callback; }

    ModeType  getMode() { return m_Mode; }

    // Triggered
    CA2D::DataType getData();

    // Continuous
    static void         ISR_Data();
    inline BlockType*   getBlockToSend()     { return m_pBlockToSend;         }
    inline void         releaseBlockToSend() { m_pBlockToSend->data->clear(); }
    volatile bool       isBlockReadyToSend = false;

  private:
    void      setMode(CA2D::ModeType mode);
    void      setMode_Continuous();
    void      setMode_Triggered();

    CA2D::DataType readData();
    void           parseData();

    InputPin            m_pinDataReady{9};

  private:
    static SPISettings  g_settings;

    ModeType            m_Mode       = ModeType::UNSET;
    CallbackType        m_fnCallback = NULL;

    // Continuous
    CHead::StateType    m_State;

    BlockType           m_BlockA;
    BlockType           m_BlockB;

    BlockType* volatile m_pBlockToFill;
    BlockType* volatile m_pBlockToSend;

    void SPIwrite(std::initializer_list<uint8_t> data);
}; 