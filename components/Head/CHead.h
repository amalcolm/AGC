#pragma once
#include <Arduino.h>

class CHead {
  public:
    typedef uint32_t StateType;
    static const StateType DIRTY = 0xFFFFFFFF;

    //                                    3         2         1         0
    //                                   10987654321098765432109876543210

    //                                             IR             RED
    //                                          987654321       987654321

    static const StateType VALID     = 0b00000001111111110000000111111111; 

    static const StateType ALL_OFF   = 0b00000000000000000000000000000000;
    static const StateType ALL_ON    = 0b00000001111111110000000111111111;

    static const StateType RED1      = 0b00000000000000000000000000000001;
    static const StateType IR1       = 0b00000000000000010000000000000000;

    static const StateType UNSET     = 0b10000000000000000000000000000000;


  private:
    uint32_t* m_pSequence;

    int   m_sequenceLength   = -1;
    int   m_sequencePosition = -1;

  

  public:
    CHead();
   ~CHead();
   
    void init();
    void setSequence( std::initializer_list<uint32_t> data );

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