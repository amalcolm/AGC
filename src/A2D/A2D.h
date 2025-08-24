#pragma once
#include <SPI.h>
#include "Head/Head.h"
#include "Setup.h"
#include <vector>

class CA2D {
  public:
    enum ModeType { UNSET, CONTINUOUS, TRIGGERED };

    #include "A2D_DataTypes.h"

  public:
    CA2D(ModeType mode);
    CA2D(CA2D::CallbackType callback);
    
    void      setHead    (CHead*             pHead   ) { m_pHead = pHead;         }
    void      setCallback(CA2D::CallbackType callback) { m_fnCallback = callback; }

    ModeType  getMode() { return m_Mode ; };

    // Triggered
    void           GPIOwrite(uint8_t b);
    CA2D::DataType getData ();

    // Continuous
    static void         ISR_Data();
    volatile BlockType* getBlockToSend();
    void                releaseBlockToSend();
    volatile bool      isBlockReadyToSend = false;

  private:
    void      setMode(CA2D::ModeType mode);
    void      setMode_Continuous();
    void      setMode_Triggered();

    CA2D::DataType readData();
    void           parseData();

    InputPin<9> m_pinDataReady;

  private:
    static SPISettings  g_settings;

    ModeType            m_Mode       = ModeType::UNSET;
    CallbackType        m_fnCallback = NULL;
    CHead*              m_pHead      = NULL;

    // Continious
    CHead::StateType    m_State;

    BlockType           m_BlockA;
    BlockType           m_BlockB;

    volatile BlockType *m_pBlockToFill;
    volatile BlockType *m_pBlockToSend;

    void SPIwrite(std::initializer_list<uint8_t> data);
}; 