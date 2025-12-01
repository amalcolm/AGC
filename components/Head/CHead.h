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
    static constexpr StateType RED2      = 0b00000000000000000000000000000010;
    static constexpr StateType RED3      = 0b00000000000000000000000000000100;
    static constexpr StateType RED4      = 0b00000000000000000000000000001000;
    static constexpr StateType RED5      = 0b00000000000000000000000000010000;
    static constexpr StateType RED6      = 0b00000000000000000000000000100000;
    static constexpr StateType RED7      = 0b00000000000000000000000001000000;
    static constexpr StateType RED8      = 0b00000000000000000000000010000000;
    static constexpr StateType RED9      = 0b00000000000000000000000100000000;

    //                                                 IR             RED
    //                                              987654321       987654321

    static constexpr StateType IR1       = 0b00000000000000010000000000000000;
    static constexpr StateType IR2       = 0b00000000000000100000000000000000;
    static constexpr StateType IR3       = 0b00000000000001000000000000000000;
    static constexpr StateType IR4       = 0b00000000000010000000000000000000;
    static constexpr StateType IR5       = 0b00000000000100000000000000000000;
    static constexpr StateType IR6       = 0b00000000001000000000000000000000;
    static constexpr StateType IR7       = 0b00000000010000000000000000000000;
    static constexpr StateType IR8       = 0b00000000100000000000000000000000;
    static constexpr StateType IR9       = 0b00000001000000000000000000000000;

    //                                                 IR             RED
    //                                              987654321       987654321

    static constexpr StateType VALIDBITS = 0b00000001111111110000000111111111; 

    static constexpr StateType ALL_OFF   = 0b00000000000000000000000000000000;
    static constexpr StateType ALL_ON    = 0b00000001111111110000000111111111;

    static constexpr StateType UNSET     = 0b10000000000000000000000000000000;


  
  public:
    CHead();
   ~CHead();
   
    void begin();
    void setSequence( std::vector<StateType> data );
    void setSequence(std::initializer_list<StateType> il);

    inline StateType getState() { return m_State; }
    StateType setNextState();
    void clear();

    std::vector<StateType>& getSequence();


  private:
    StateType   m_State;

    std::vector<StateType> m_sequence;
    int   m_sequencePosition = -1;
  
};