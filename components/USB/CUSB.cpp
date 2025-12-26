#include "CUSB.h"
#include "DataTypes.h"
#include "CHead.h"
#include "CTelemetry.h"
#include "Config.h"
#include <map>

constexpr unsigned int TEXTOUT_INTERVAL = 10000; // 10ms

CUSB::CUSB() : m_buffer{}, m_pBlock(nullptr), writeIndex(0), readIndex(0) {}

void CUSB::begin() { CSerialWrapper::begin(); }


void CUSB::buffer(DataType data) {
  
  uint8_t nextIndex = (writeIndex + 1) % BUFFER_SIZE;

  if (nextIndex != readIndex) {
    m_buffer[writeIndex] = data;
    writeIndex = nextIndex;
  }
}

void CUSB::buffer(BlockType* pBlock) {
  if (pBlock == nullptr || pBlock->state == DIRTY) return;
  m_pBlock = pBlock;
}

void CUSB::buffer(CTelemetry* telemetry) {
  telemetryBuffer.emplace(telemetry);
}

void filter(BlockType* block) {
  
  if (block == nullptr || block->count < 2) return;

  // If first reading is less than 1/8 of second, remove it
  if (block->data[0].channels[0] < block->data[1].channels[0] / 8) {
    // delete first entry
    for (uint32_t i = 1; i < block->count; ++i) {
      block->data[i - 1] = block->data[i];
    }
    block->count--;
  }
  
}

// Called from the main loop: sends data from the buffer.
void CUSB::update() {

  CSerialWrapper::update();
 
  switch (getMode())
  {
    case CSerialWrapper::ModeType::BLOCKDATA:
    {
      if (m_pBlock == NULL) return;
      
      filter(m_pBlock);

      if (m_handshakeComplete)
        m_pBlock->writeSerial();
      else
        if (A2D.outputDebugBlock)
          m_pBlock->debugSerial();

      m_pBlock = NULL;
      break;
    }

    case CSerialWrapper::ModeType::TEXT:
    {
      if (CFG::TESTMODE) {
        BlockType block;
        A2D.makeCallback(&block);
      }
      break;
    }

    case CSerialWrapper::ModeType::RAWDATA:
    {
      while (readIndex != writeIndex) {//  if (firstOut == 0) firstOut = m_buffer[readIndex].timestamp;
        DataType *pData = &m_buffer[readIndex];

        if (m_handshakeComplete)
          pData->writeSerial();
        else
          pData->debugSerial();

        readIndex = (readIndex + 1) % BUFFER_SIZE;
      }
    
      break;
    }

    default:
      break;
  }

  while (!telemetryBuffer.empty()) {
      CTelemetry* telemetry = telemetryBuffer.front();

      if (m_handshakeComplete)
          telemetry->writeSerial();
        
      CTelemetry::Return(telemetry);
      telemetryBuffer.pop();
  }

}
#include <String.h>  // Usually already available in Teensy core


class CUSB::CStringPrinter : public Print {
public:
  CStringPrinter(String &output) : out(output) {}

  // Called by Print::write() — must return the number of bytes written
  size_t write(uint8_t b) override {
    out += static_cast<char>(b);
    return 1;
  }

  // Optional: handle bulk writes for efficiency
  size_t write(const uint8_t *buffer, size_t size) override {
    out.reserve(out.length() + size);
    for (size_t i = 0; i < size; ++i) {
      out += static_cast<char>(buffer[i]);
    }
    return size;
  }

private:
  String &out;
};

void CUSB::SendCrashReport(CrashReportClass& pReport)
{
  Serial.begin(115200);
  for (int i = 0; i < 50 && !Serial; ++i) delay(10); 
  
  const char header[] = "\n\n***** CRASH REPORT *****\n";

  String fullReport{};
  fullReport.reserve(1536);  // Generous—covers even deep stack dumps
  CStringPrinter printer(fullReport);
  pReport.printTo(printer); 

  while (true) {
    Serial.print(header);
    Serial.print(fullReport);

    Pins::flash(5);
    delay(1000);
  }
}

