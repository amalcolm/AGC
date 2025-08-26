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
  // set as input to give access to current state. !! Important !! - may overheat chip if ommitted.
  //  pinMode(PIN_GPIO_RED, INPUT);  digitalWrite(PIN_GPIO_RED, LOW);
  //  pinMode(PIN_GPIO_IR , INPUT);  digitalWrite(PIN_GPIO_IR , LOW);lk

  switch (mode) {
    case CA2D::ModeType::CONTINUOUS: setMode_Continuous();  break;
    case CA2D::ModeType::TRIGGERED : setMode_Triggered ();  break;
    default: error("*** A2D Mode %d not configured ***");  break;
  }
}

void CA2D::init() {
  m_pinDataReady.init();
}

CA2D::DataType CA2D::readData() {

  DataType data{m_pHead->getState()};

  if (getMode() == ModeType::CONTINUOUS)
      digitalWrite(CS.A2D, LOW);

  uint8_t rawData[27];

  for( byte &b : rawData)
    b = SPI.transfer(0x00);
  
  digitalWrite(CS.A2D, HIGH);

  if (m_pHead->getState() != data.State)
    data.State = CHead::DIRTY;

  uint8_t *pR = rawData;
  uint8_t b2 = *pR++, b1 = *pR++, b0 = *pR++;


  for(int q = 1 ; q < 7 ; q++){
    b2 = *pR++, b1 = *pR++, b0 = *pR++;
    long rawValue = (b2 << 16) | (b1 << 8) | b0;

    if (rawValue & 0x800000)
      rawValue -= 0x1000000;

    data.Channels[q] = rawValue;
  }
  
  return data;
}


void CA2D::GPIOwrite(byte b) {
  SPIwrite( { 0x54, 0x00, b });
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
