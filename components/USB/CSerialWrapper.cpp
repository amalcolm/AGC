#include "WString.h"
#include "CSerialWrapper.h"
#include "CHead.h"
#include "CTimer.h"
#include "Setup.h"
#include "Hardware.h"
#include "helpers.h"
#include <array>


CSerialWrapper::CSerialWrapper() : m_Mode(ModeType::UNSET), m_handshakeComplete(false) {

  Serial.begin(USB_BAUDRATE);
}

void CSerialWrapper::tick() {
  static bool lastConnected = false;
  bool connected = (bool)Serial; // or (Serial && Serial.dtr()) if you want to be strict

  // Detect disconnect edge
  if (lastConnected && !connected && getMode() != ModeType::UNSET) {
    Pins::flash(3);
    m_handshakeComplete = false;
    setMode(ModeType::UNSET);
  }

  lastConnected = connected;

  if (Serial.available() > 0) {
    if (!m_handshakeComplete)
      begin();
    while (Serial.available() > 0) Serial.read();
  }

  if (connected && getMode() == ModeType::UNSET)
    begin();
}

void CSerialWrapper::begin() {
  static const unsigned long timeout = 1000;  // timeout (mS)
  setMode(ModeType::INITIALISING);

  unsigned long endTime = millis() + timeout;
  m_handshakeComplete = false;
  const char* target = "HOST_ACK";
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
          
          Serial.write("DEVICE_ACK\n");
          endTime += timeout;
          Serial.send_now();
          HOST_VERSION.clear();
          HOST_VERSION.reserve(32);
          while (!m_handshakeComplete && (millis() < endTime)) {
            if (Serial.available())
            {
              c = Serial.read();

              if (c != '\n') 
                HOST_VERSION += c;
              else {
                Serial.write(DEVICE_VERSION.c_str());
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


  if (m_handshakeComplete)
    setMode(ModeType::BLOCKDATA);
  else
    setMode(ModeType::TEXT);

  Serial.printf("Device Started %s v%s\n%20s\n", getSketch(), DEVICE_VERSION.c_str(), __DATE__);
  if (m_handshakeComplete)
    Serial.printf("Handshake complete. Host version: %s\n", HOST_VERSION.c_str());
  else 
    Serial.println("Handshake failed. Continuing in TEXT mode.\n");
  

  Head.clear(); // ensure all LEDs off at start
  Serial.flush(); // ensure all output sent
  Serial.clear(); // clear output buffer
  while (Serial.available() > 0) Serial.read(); // flush input buffer
  Timer.resetStartMillis();

}

CSerialWrapper::ModeType CSerialWrapper::setMode(CSerialWrapper::ModeType mode) {
  m_Mode = mode;
  return m_Mode;
}

void CSerialWrapper::write(uint8_t  byte) { put(&byte,           sizeof(byte)); }
void CSerialWrapper::write(uint32_t data) { put((uint8_t*)&data, sizeof(data)); }

void CSerialWrapper::write(uint8_t* pData, uint32_t dataLen) { put(pData, dataLen); }


inline void CSerialWrapper::put(uint8_t* pData, uint32_t dataLen) {
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


 