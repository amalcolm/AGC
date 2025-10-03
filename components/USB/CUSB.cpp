#include "CUSB.h"
#include "CA2D_DataTypes.h"
#include "CHead.h"
#include <map>


CUSB::CUSB() {}

void CUSB::init() { CSerialWrapper::init(); }  



void CUSB::buffer(CA2D::DataType data) {
  
  uint8_t nextIndex = (writeIndex + 1) % BUFFER_SIZE;

  if (nextIndex != readIndex) {
    m_buffer[writeIndex] = data;
    writeIndex = nextIndex;
  }
}

void CUSB::buffer(volatile CA2D::BlockType* pBlock) {
  m_pBlock = pBlock;
}

// Called from the main loop: sends data from the buffer.
void CUSB::output_buffer() {
  constexpr unsigned int TEXTOUT_INTERVAL = 10000; // ms

  CSerialWrapper::ModeType mode = getMode();

  if (mode == CSerialWrapper::ModeType::BLOCKDATA) {
      if (m_pBlock == NULL) return;
      writeRawData(m_pBlock);
      m_pBlock = NULL;
      return;
  }

  
  static uint32_t lastTick = 0;
  static uint32_t lastOutTime = 0;
  static std::map<CHead::StateType, int> lastReading;

  while (readIndex != writeIndex) {//  if (firstOut == 0) firstOut = m_buffer[readIndex].timeStamp;
    CA2D::DataType *pData = &m_buffer[readIndex];
    pData->timeDelta = pData->timeStamp - lastTick;
    lastTick = pData->timeStamp;


    switch (mode)
    {
      case CSerialWrapper::ModeType::BLOCKDATA: // handled above
        break;

      case CSerialWrapper::ModeType::RAWDATA:
        writeRawData(pData);
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

