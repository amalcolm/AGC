#include "WString.h"
#include "CSerialWrapper.h"
#include "Setup.h"
#include "helpers.h"
#include <map>

std::map<CSerialWrapper::ModeType, std::array<uint8_t, CSerialWrapper::FRAMING_SIZE>> g_StartFrame;
std::map<CSerialWrapper::ModeType, std::array<uint8_t, CSerialWrapper::FRAMING_SIZE>> g_EndFrame;

CSerialWrapper::CSerialWrapper()  {
  static const unsigned long USB_BAUDRATE = 57600 * 16;

  Serial.begin(USB_BAUDRATE);

  g_StartFrame[CSerialWrapper::BLOCKDATA] = m_BlockData_Start;
  g_EndFrame[  CSerialWrapper::BLOCKDATA] = m_BlockData_End;
  g_StartFrame[CSerialWrapper::RAWDATA] = m_RawData_Start;
  g_EndFrame[  CSerialWrapper::RAWDATA] = m_RawData_End;

}

void CSerialWrapper::init() {}  // serialWtapper is initialized when 

bool CSerialWrapper::isInitialising() {
  if (getMode() == ModeType::UNSET) begin();
  return getMode() == ModeType::INITIALISING;
}


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


  if (m_handshakeComplete)
    setMode(ModeType::BLOCKDATA);
  else
    setMode(ModeType::TEXT);
}

CSerialWrapper::ModeType CSerialWrapper::setMode(CSerialWrapper::ModeType mode) {
  if ((m_Mode == mode) || !m_handshakeComplete) return m_Mode;

  if (m_Mode & FRAMING_MASK)
    put(  g_EndFrame[m_Mode].data(), CSerialWrapper::FRAMING_SIZE);

  m_Mode = mode;

  if (m_Mode & FRAMING_MASK)
    put(g_StartFrame[m_Mode].data(), CSerialWrapper::FRAMING_SIZE);

  return m_Mode;
}

void CSerialWrapper::put(uint8_t* pData, unsigned int dataLen) {
  Serial.write(pData, dataLen); 
}


void CSerialWrapper::printf(const char *pFMT, ...) {
  ModeType previousMode = getMode();
  setMode(ModeType::TEXT);

  char buffer[PRINTF_BUFFER_SIZE];
  va_list args;
  va_start(args, pFMT);
  vsnprintf(buffer, sizeof(buffer)-1, pFMT, args);
  va_end(args);
  put((uint8_t*)buffer, strlen(buffer));

  setMode(previousMode);
}


 