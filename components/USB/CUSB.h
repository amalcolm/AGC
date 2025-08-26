#pragma once
#include "CSerialWrapper.h"

class CUSB : public CSerialWrapper {
  private:
    static const int BUFFER_SIZE = 256;
    CA2D::DataType m_buffer[BUFFER_SIZE];
    volatile CA2D::BlockType* m_pBlock;
    
    volatile uint8_t writeIndex = 0;
    volatile uint8_t readIndex = 0;

  public:
    CUSB();

    void init();
    
    void buffer(CA2D::DataType data);
    void buffer(volatile CA2D::BlockType* block);
    
    void output_buffer();
};
