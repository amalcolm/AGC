#include "CA2D.h"
#include "Setup.h"
#include "CHead.h"
#include "Helpers.h"
#include "Hardware.h"
#include "CTimer.h"
CA2D::CA2D(ModeType mode) : m_Mode(mode) {}

CA2D::CA2D(CallbackType callback) : m_Mode(CONTINUOUS), m_fnCallback(callback) {}

void CA2D::setMode(CA2D::ModeType mode) {
  pinMode(PIN_SPI_SCK   , OUTPUT); // SPI SCK
  pinMode(PIN_SPI_MOSI  , OUTPUT); // SPI MOSI
  pinMode(PIN_SPI_MISO  , INPUT ); // SPI MISO
  pinMode(CS.A2D        , OUTPUT); // SPI CS
  pinMode(m_pinDataReady, INPUT ); // no pullups; ADS drives the line

  // reset SPI and CS pins
  digitalWrite(CS.A2D, HIGH);
  delayMicroseconds(3);
  digitalWrite(CS.A2D, LOW);
  delayMicroseconds(3);
  digitalWrite(CS.A2D, HIGH);
  delayMicroseconds(3);

  switch (mode) {
    case CA2D::ModeType::CONTINUOUS: setMode_Continuous();  break;
    case CA2D::ModeType::TRIGGERED : setMode_Triggered ();  break;
    default:  break;
  }
}

CA2D& CA2D::begin() {
  setMode(m_Mode);
  return *this;
}

// Call with CS already LOW (continuous). Return false if header not found.
bool CA2D::readFrame(uint8_t (&raw)[32]) {
  
  // Read status first
  for (int i=0;i<3;i++) raw[i] = SPI.transfer(0x00);

  // Header check per datasheet: top nibble of first status byte = 1100b (0xC?)
  if ((raw[0] & 0xF0) != 0xC0) {
    // Drain remaining bytes for this bad frame so we realign next time
    for (int i=3;i<27;i++) (void)SPI.transfer(0x00);
    return false;
  }

  // Read 8 channels × 3 bytes
  for (int i=3;i<27;i++) raw[i] = SPI.transfer(0x00);
  return true;
}




// 24-bit sign-extend (two’s-complement)
inline int32_t be24_to_s32(const uint8_t b2, const uint8_t b1, const uint8_t b0) {
  int32_t v = (int32_t(b2) << 16) | (int32_t(b1) << 8) | int32_t(b0);
  if (v & 0x00800000) v |= 0xFF000000; // sign-extend bit 23
  return v;
}

void CA2D::dataFromFrame(uint8_t (&raw)[32], DataType& data) {
  static uint8_t sequenceNumber = 0;

  data.timestamp = Timer.getConnectTime();
  data.stateTime = Timer.getStateTime();

  const uint8_t* p = &raw[3]; // skip status
  for (int ch=0; ch<8; ++ch) {
    int32_t val = be24_to_s32(p[0], p[1], p[2]);
    p += 3;
    data.channels[ch] = val;
  }

  auto& [state, offsetPot1, offsetPot2, gainPot, tele] = getPerStateHW(data);   IGNORE(tele);
  
  data.hardwareState = 
      (sequenceNumber++      << 24) |
      (offsetPot1.getLevel() << 16) |
      (offsetPot2.getLevel() <<  8) |
      (gainPot   .getLevel()      );

  data.sensorState =
      (analogRead(offsetPot1.getSensorPin()) << 16) |
      (analogRead(offsetPot2.getSensorPin())      );

}


DataType CA2D::readData() {

  DataType data(Head.getState());

  uint8_t raw[32];
  bool ok = readFrame(raw);
  if (ok) 
    dataFromFrame(raw, data);
  else {
    data.state = DIRTY;
    return data;
  }

  return data;
}



void CA2D::SPIwrite(std::initializer_list<uint8_t> data) {
  digitalWrite(CS.A2D, LOW);
  delayMicroseconds(5);
  for (uint8_t b : data) {
    SPI.transfer(b);
  }
  delayMicroseconds(5);
  digitalWrite(CS.A2D, HIGH);
  delayMicroseconds(10);
}

