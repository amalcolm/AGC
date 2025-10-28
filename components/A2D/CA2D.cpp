#include "CA2D.h"
#include "Setup.h"
#include "CHead.h"

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

CA2D& CA2D::init() {
  setMode(m_Mode);
  return *this;
}


void CA2D::poll() { if (m_Mode == ModeType::CONTINUOUS) pollData(); }

// Call with CS already LOW (continuous). Return false if header not found.
bool CA2D::readFrame(uint8_t (&raw)[27]) {
  // ensure status bits are set
  delayMicroseconds(2);
  
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


void CA2D::dataFromFrame(uint8_t (&raw)[27], DataType& data) {
  const uint8_t* p = &raw[3]; // skip status
  for (int ch=0; ch<8; ++ch) {
    int32_t val = be24_to_s32(p[0], p[1], p[2]);
    p += 3;
    data.channels[ch] = val;
  }
}


DataType CA2D::readData() {

  DataType data(CHead::getActiveState());

  uint8_t raw[27]; 
  bool ok = readFrame(raw);

  if (ok) 
    dataFromFrame(raw, data);
  
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

SPISettings    CA2D::g_settings(4800000, MSBFIRST, SPI_MODE1);
