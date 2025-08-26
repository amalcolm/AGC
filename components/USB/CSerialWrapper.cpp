#include "WString.h"
#include "CSerialWrapper.h"
#include "Setup.h"
#include "helpers.h"
#include <map>

CSerialWrapper::CSerialWrapper()  {
  Serial.begin(57600*16);
}

void CSerialWrapper::init() {}

bool CSerialWrapper::isInitialising() {
  if (getMode() == ModeType::UNSET) begin();
  return getMode() == ModeType::INITIALISING;
}


std::map<CSerialWrapper::ModeType, std::array<uint8_t, CSerialWrapper::FRAMING_SIZE>> g_StartFrame;
std::map<CSerialWrapper::ModeType, std::array<uint8_t, CSerialWrapper::FRAMING_SIZE>> g_EndFrame;

void CSerialWrapper::begin() {
  static const unsigned long timeout = 1000;  // timeout (mS)
  setMode(ModeType::INITIALISING);

  // assumes serial port is open and ready (done in constructor)
  Serial.printf("\n\nTeensy 4.1\n Started %s v%.2lf\n%20s\n", getSketch(), VERSION, __DATE__);
  
  // Clear buffer
  while(Serial.available()) {
    Serial.read();
  }
 
  unsigned long endTime = millis() + timeout;
  m_handshakeComplete = false;
  
  const char* target = "HOST_ACK";
  const uint8_t targetLength = 7;
  uint8_t targetIndex = 0;
  
  while (!m_handshakeComplete && (millis() < endTime)) {
    if (Serial.available() > 0) {
      char c = Serial.read();
      
      // Match character against current position in target
      if (c == target[targetIndex]) {
        targetIndex++;
        
        if (targetIndex >= targetLength) {
          m_handshakeComplete = true;
          Serial.println("HANDSHAKE_COMPLETE");
          break;
        }
      } else {
        // Reset match but check if current char matches first char of target
        targetIndex = (c == target[0]) ? 1 : 0;
      }
    }
  }


  g_StartFrame[CSerialWrapper::PACKETS] = m_Packet_Start;
  g_EndFrame[  CSerialWrapper::PACKETS] = m_Packet_End;
  g_StartFrame[CSerialWrapper::RAWDATA] = m_RawData_Start;
  g_EndFrame[  CSerialWrapper::RAWDATA] = m_RawData_End;


  setMode(ModeType::PACKETS);
}

CSerialWrapper::ModeType CSerialWrapper::setMode(CSerialWrapper::ModeType mode) {
  if (m_Mode == mode) return m_Mode;

  if ((m_Mode & FRAMING_MASK) && m_handshakeComplete)
    put(g_EndFrame[m_Mode].data(), CSerialWrapper::FRAMING_SIZE);

  m_Mode = mode;

  if ((m_Mode & FRAMING_MASK) && m_handshakeComplete)
    put(g_EndFrame[m_Mode].data(), CSerialWrapper::FRAMING_SIZE);

  return m_Mode;
}

void CSerialWrapper::put(uint8_t* pData, uint dataLen) {
  Serial.write(pData, dataLen); 
}


void CSerialWrapper::printf(const char *pFMT, ...) {        
  setMode(ModeType::PACKETS);

  char buffer[PRINTF_BUFFER_SIZE];
  va_list args;
  va_start(args, pFMT);
  vsnprintf(buffer, sizeof(buffer)-1, pFMT, args);
  va_end(args);
  put((uint8_t*)buffer, strlen(buffer));

  setMode(CSerialWrapper::ModeType::UNSET);
}

void CSerialWrapper::writeRawData(CA2D::DataType* pData) {  if (pData == NULL) return;
 
  if (m_Mode != ModeType::RAWDATA) 
    setMode(ModeType::RAWDATA);

  put((uint8_t*)pData, sizeof(CA2D::DataType));

  // leave it in rawdata mode
}

void CSerialWrapper::writeRawData(volatile CA2D::BlockType* pBlock) {  if (pBlock == NULL) return;
  
  if (m_Mode != ModeType::BLOCKDATA)
    setMode(ModeType::BLOCKDATA);

  CA2D::BlockType* nvpBlock = const_cast<CA2D::BlockType*>(pBlock);

  nvpBlock->writeSerial();

  // leave it in blockdata mode
}

 