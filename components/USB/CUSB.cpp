#include "CUSB.h"
#include "DataTypes.h"
#include "CHead.h"
#include <map>


CUSB::CUSB() {}

void CUSB::init() { CSerialWrapper::init(); }  



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
void CUSB::output_buffer() {
  constexpr unsigned int TEXTOUT_INTERVAL = 10000; // ms

  auto mode = getMode();

  if (mode == CSerialWrapper::ModeType::BLOCKDATA) {
      if (m_pBlock == NULL) return;
      if (m_handshakeComplete)
        m_pBlock->writeSerial();
      else
        m_pBlock->debugSerial();

      m_pBlock = NULL;
      return;
  }

  
//static uint32_t lastTick = 0;
  static uint32_t lastOutTime = 0;
  static std::map<StateType, int> lastReading;

  while (readIndex != writeIndex) {//  if (firstOut == 0) firstOut = m_buffer[readIndex].timeStamp;
    DataType *pData = &m_buffer[readIndex];
//  lastTick = pData->timeStamp;


    switch (mode)
    {
      case CSerialWrapper::ModeType::BLOCKDATA: // handled above
        break;

      case CSerialWrapper::ModeType::RAWDATA:
        if (m_handshakeComplete)
          pData->writeSerial();
        else
          pData->debugSerial();
        break;


      case CSerialWrapper::ModeType::TEXT:
        lastReading[pData->state] = pData->channels[1];

        // if nothing has been output for a while, switch to debug output
        if (millis() - lastOutTime > TEXTOUT_INTERVAL) {
          lastOutTime = millis();

          printf("Amb:%d\tRED:%d\tIR:%d\n", lastReading[CHead::ALL_OFF], lastReading[CHead::RED1], lastReading[CHead::IR1]);
        }

        break;

      default:
        break;
    }

    readIndex = (readIndex + 1) % BUFFER_SIZE;
  }
}


void CUSB::CrashReport(CrashReportClass& pReport)
{
  Serial.begin(115200);
  for (int i = 0; i < 50 && !Serial; ++i) delay(10); 

  pReport.printTo(Serial);

  while (true) {
    Pins::flash(2);
  }
}