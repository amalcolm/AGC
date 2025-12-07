#include "CUSB.h"
#include "DataTypes.h"
#include "CHead.h"
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
  m_pBlock = pBlock;
}

// Called from the main loop: sends data from the buffer.
void CUSB::tick() {

  CSerialWrapper::tick();
 
  switch (getMode())
  {
    case CSerialWrapper::ModeType::BLOCKDATA:
    {
      if (m_pBlock == NULL) return;
      
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
}

void CUSB::SendCrashReport(CrashReportClass& pReport)
{
  Serial.begin(115200);
  for (int i = 0; i < 50 && !Serial; ++i) delay(10); 

  pReport.printTo(Serial);

  while (true) {
    Pins::flash(5);
    delay(1000);
  }
}