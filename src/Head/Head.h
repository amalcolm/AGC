#pragma once
#include <Arduino.h>

class CHead {
  public:
    typedef uint StateType;
    static const StateType DIRTY = 0xFFFFFFFF;

    static const uint8_t LEDS_OFF = 0x00;     static const uint8_t GPIO_OFF = 0xF0;
    static const uint8_t LED_RED  = 0x01;     static const uint8_t GPIO_RED = 0xE0;
    static const uint8_t LED_IR   = 0x02;     static const uint8_t GPIO_IR  = 0xD0;

    static const uint8_t UNSET    = LEDS_OFF;


    static constexpr uint8_t dLookupIO[8] { GPIO_RED, GPIO_IR, 0,0, 0,0,0,0 };




  private:
    uint8_t* m_pSequence;

    int   m_sequenceLength   = -1;
    int   m_sequencePosition = -1;

  

  public:
    CHead();
   ~CHead();
   
    void setSequence( std::initializer_list<uint8_t> data );

    StateType getState() { return m_State; };
    StateType setNextState();

    inline static StateType getActiveState();

  private:
    StateType   m_State;
    






/*
    static uint32_t  m_portGPIO ;
    static uint32_t  m_maskGPIO_RED;
    static uint32_t  m_maskGPIO_IR;
    volatile static uint32_t* m_portInputRegister;
    static void      setPortAndMasks();
*/
};