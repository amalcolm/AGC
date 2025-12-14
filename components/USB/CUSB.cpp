#include "CUSB.h"
#include "DataTypes.h"
#include "CHead.h"
#include "CTelemetry.h"
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
      if (TESTMODE) {
        BlockType block;
        A2D.makeCallback(&block);
      }
      break;
    }

    case CSerialWrapper::ModeType::RAWDATA:
    {
      while (readIndex != writeIndex) {//  if (firstOut == 0) firstOut = m_buffer[readIndex].timeStamp;
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

void CUSB::SendCrashReport(CrashReportClass& pReport)
{
  Serial.begin(115200);
  for (int i = 0; i < 50 && !Serial; ++i) delay(10); 

  char header[] = "\n\n--- CRASH REPORT ---\n";

  while (true) {
    Serial.write((uint8_t*)header, sizeof(header) - 1);
    pReport.printTo(Serial);
    Pins::flash(5);
    delay(1000);
  }
}