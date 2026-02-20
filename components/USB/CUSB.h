#pragma once
#include "CSerialWrapper.h"
#include "CrashReport.h"
#include "CBuffer.h"

class CUSB : public CSerialWrapper {
  private:
    static constexpr size_t      BYTE_BUFFER_SIZE = 1024;  // holds raw bytes read from USB    (read)  (1024 bytes)
    static constexpr size_t  DATATYPE_BUFFER_SIZE =   64;  // holds data for buffer(dataType); (write) (64 * sizeof(DataType) = 1.5KB)
    static constexpr size_t TELEMETRY_BUFFER_SIZE =  128;  // holds pointers to CTelemetry items to write

    BufferType<uint8_t>   m_byteBuffer = BufferType<uint8_t >(    BYTE_BUFFER_SIZE);
    BufferType<DataType>  m_dataBuffer = BufferType<DataType>(DATATYPE_BUFFER_SIZE);
    BlockType* m_pBlock     = nullptr;  // single block is buffered, (see implementation in CA2D - swapps between two blocks)
    
    BufferType<CTelemetry*> m_telemetryBuffer = BufferType<CTelemetry*>(TELEMETRY_BUFFER_SIZE);  // stores pointers, not actual items

  public:
    CUSB() {};

    void begin()
    { 
      CSerialWrapper::begin(); 
    }
    
    inline void buffer(DataType    data     ) { m_dataBuffer.write(data); }
    inline void buffer(BlockType*  block   ) { m_pBlock = block; }
    inline void buffer(CTelemetry* telemetry) { m_telemetryBuffer.write(telemetry); }
    
    void update() { 

      CSerialWrapper::update();

      do_read();
      do_write();

    };

    void clearBuffers() {
      m_byteBuffer.clear();
      m_dataBuffer.clear();
      m_telemetryBuffer.clear();
    }

    static void SendCrashReport(CrashReportClass& pReport);

  private:
    void do_read();
   
    void do_write();
    void do_write_Data();
    void do_write_Block();
    void do_write_Text();
    void do_write_Telemetry();
    
};
