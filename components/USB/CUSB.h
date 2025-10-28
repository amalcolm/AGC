#pragma once
#include "CSerialWrapper.h"

class CUSB : public CSerialWrapper {
  private:
    static const int BUFFER_SIZE = 256;
    DataType m_buffer[BUFFER_SIZE];
    volatile BlockType* m_pBlock;
    
    volatile uint8_t writeIndex = 0;
    volatile uint8_t readIndex = 0;

  public:
    CUSB();

    void init();
    
    void buffer(DataType data);
    void buffer(volatile BlockType* block);
    
    void output_buffer();
};
