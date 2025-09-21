#include "CA2D.h"
#include "Setup.h"
#include "helpers.h"

CA2D::CA2D(ModeType mode) { 
  setMode(mode);
}

CA2D::CA2D(CA2D::CallbackType callback) : m_fnCallback(callback) {
  setMode(ModeType::CONTINUOUS);
}

void CA2D::setMode(CA2D::ModeType mode) {
  switch (mode) {
    case CA2D::ModeType::CONTINUOUS: setMode_Continuous();  break;
    case CA2D::ModeType::TRIGGERED : setMode_Triggered ();  break;
    default: error("*** A2D Mode %d not configured ***");  break;
  }
}

CA2D* CA2D::init() {
  m_pinDataReady.init();
  return this;
}

CA2D::DataType CA2D::readData() {

  DataType data{Head.getState()};

  uint8_t raw[27];
  unsigned int err = read_frame(raw);

  if (!err) {
    const uint8_t* p = &raw[3]; // skip status
    for (int ch=0; ch<8; ++ch) {
      int32_t val = be24_to_s32(p[0], p[1], p[2]);
      p += 3;
      data.Channels[ch] = val;
    }
  }
  else
    data.State = 0x1000 | err; // mark as dirty with error code in low byte
   
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
