#include "WString.h"
#include "CSerialWrapper.h"
#include "CSerialStateMachine.h"
#include "CHead.h"
#include "CTimer.h"
#include "Setup.h"
#include "Hardware.h"
#include "helpers.h"
#include <array>
#include <queue>

CSerialWrapper::CSerialWrapper() : m_Mode(ModeType::UNSET), stateMachine(SerialStateMachine::create()), m_handshakeComplete(false) {
  Serial.begin(USB_BAUDRATE);
}

void CSerialWrapper::update() {
  bool connected = Serial;
  bool hasData = Serial.available() > 0;

  stateMachine.update(connected, hasData, TESTMODE, m_handshakeComplete, m_Mode, [this]() { this->doHandshake(); });
}

void CSerialWrapper::begin() {
  update();
}

void CSerialWrapper::doHandshake() {
  static const unsigned long timeout = 1000;  // timeout (mS)
  setMode(ModeType::INITIALISING);

  unsigned long endTime = millis() + timeout;
  m_handshakeComplete = false;
  const char* target = ">HOST_ACK";
  const uint8_t targetLength = strlen(target);
  uint8_t targetIndex = 0;
  
  while (!m_handshakeComplete && (millis() < endTime)) {
    if (Serial.available() > 0) {
      char c = Serial.read();
      
      // Match character against current position in target
      if (c == target[targetIndex]) {
        targetIndex++;
        
        if (targetIndex >= targetLength) {

          Serial.clear(); // clear output buffer
          while (Serial.available()) Serial.read(); // flush input buffer
          
          Serial.write("<DEVICE_ACK\n");
          endTime += timeout;
          Serial.send_now();
          HOST_VERSION.clear();
          HOST_VERSION.reserve(32);
          while (!m_handshakeComplete && (millis() < endTime)) {

            if (Serial.available())
            {
              c = Serial.read();

              if (c != '\n') {
                if (c != '>') HOST_VERSION += c;
              }
              else {
            
                Serial.write(("<"+DEVICE_VERSION).c_str());
                Serial.write("\n");
                Serial.send_now();
                m_handshakeComplete = true;
              }
            }

          }
          break;
        }
      } else {
        // Reset match but check if current char matches first char of target
        targetIndex = (c == target[0]) ? 1 : 0;
      }
    }
  }


  if (m_handshakeComplete && !TESTMODE)
    setMode(ModeType::BLOCKDATA);
  else
    setMode(ModeType::TEXT);

  std::string outcome = m_handshakeComplete ? 
    "Handshake complete. Host version: " + HOST_VERSION + (TESTMODE ? " Serial set to TEXT (by TESTMODE)" : " Binary BLOCKMODE active") + "\n" :
    "Handshake failed. Defaulting to TEXT mode.\n";

  if (m_handshakeComplete || stateMachine.getFirstCall())
    USB.printf(outcome.c_str());  

  Serial.flush(); // ensure all output sent
  Serial.clear(); // clear output buffer
  while (Serial.available() > 0) Serial.read(); // flush input buffer
  Timer.restartConnectTiming();

  stateMachine.setFirstCall(false);
//  activityLED.clear();
}

CSerialWrapper::ModeType CSerialWrapper::setMode(CSerialWrapper::ModeType mode) {
  m_Mode = mode;
  return m_Mode;
}

void CSerialWrapper::write(uint8_t  byte  ) { put(&byte,             sizeof(byte  )); }
void CSerialWrapper::write(uint16_t data  ) { put((uint8_t*)&data  , sizeof(data  )); }
void CSerialWrapper::write(uint32_t data  ) { put((uint8_t*)&data  , sizeof(data  )); }
void CSerialWrapper::write(float    data  ) { put((uint8_t*)&data  , sizeof(data  )); }
void CSerialWrapper::write(double   number) { put((uint8_t*)&number, sizeof(number)); }
  
void CSerialWrapper::write(uint8_t* pData, uint32_t dataLen) { put(pData, dataLen); }

constexpr uint32_t MAX_SETUP_QUEUE_SIZE = 64;
std::queue<std::vector<uint8_t>> setupDataQueue;

inline void CSerialWrapper::put(uint8_t* pData, uint32_t dataLen) {
  if (m_Mode == ModeType::UNSET || m_Mode == ModeType::INITIALISING)
  {
    if (setupDataQueue.size() >= MAX_SETUP_QUEUE_SIZE) return; // drop data if queue full
    setupDataQueue.emplace(pData, pData + dataLen);
    return;
  }

  while (!setupDataQueue.empty()) {
    std::vector<uint8_t>& data = setupDataQueue.front();
    Serial.write(data.data(), data.size());
    setupDataQueue.pop();
  }

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


 