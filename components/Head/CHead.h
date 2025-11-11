#pragma once
#include <initializer_list>
#include <vector>
#include "Setup.h"
#include "DataTypes.h"

class CHead {
  public:

    static constexpr int NUM_LEDS = 9;
    static constexpr int IR_STARTBIT = 16;
    static constexpr StateType DIRTY = 0xFFFFFFFF;

    //                                        3         2         1         0
    //                                       10987654321098765432109876543210

    //                                                 IR             RED
    //                                              987654321       987654321

    static constexpr StateType RED1      = 0b00000000000000000000000000000001;
    static constexpr StateType IR1       = 0b00000000000000010000000000000000;

    static constexpr StateType VALIDBITS = 0b00000001111111110000000111111111; 

    static constexpr StateType ALL_OFF   = 0b00000000000000000000000000000000;
    static constexpr StateType ALL_ON    = 0b00000001111111110000000111111111;

    static constexpr StateType UNSET     = 0b10000000000000000000000000000000;


  private:
    uint32_t* m_pSequence;

    int   m_sequenceLength   = -1;
    int   m_sequencePosition = -1;

  

  public:
    CHead();
   ~CHead();
   
    void init();
    void setSequence( std::initializer_list<uint32_t> data );

    inline StateType getState() { return m_State; }
    StateType setNextState();

    std::vector<StateType> getSequence();

  private:
    StateType   m_State;
    
};