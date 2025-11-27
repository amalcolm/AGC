#pragma once
#include "CSerialWrapper.h"
#include "CrashReport.h"

class CUSB : public CSerialWrapper {
  private:
    static const int BUFFER_SIZE = 256;
    DataType m_buffer[BUFFER_SIZE];
    BlockType* m_pBlock;
    
    volatile uint8_t writeIndex = 0;
    volatile uint8_t readIndex = 0;

  public:
    CUSB();

    void begin();
    
    void buffer(DataType data);
    void buffer(BlockType* block);
    
    void tick();

    void CrashReport(CrashReportClass& pReport);
};
