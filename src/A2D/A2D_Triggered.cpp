#include "core_pins.h"
#include "A2D.h"
#include "Head/Head.h"

void CA2D::setMode_Triggered() {
  // initalize the pins:
  pinMode(PIN_SPI_SCK , OUTPUT); //SPI SCK
  pinMode(PIN_SPI_MOSI, OUTPUT); //SPI MOSI
  pinMode(PIN_SPI_MISO, INPUT ); //SPI MISO
  pinMode(CS.A2D      , OUTPUT); //SPI CS

  SPI.begin();

  SPI.beginTransaction(g_settings);
  {
    SPIwrite({ 0x11 }); // SDATAC
    
    // CHnSe5et; 8 channels  1     2     3     4     5     6     7     8  
    SPIwrite({ 0x45, 0x07, 0x00, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61} );    // 00 = electrode, 01 =short, 05 = test sig

    // config1 register
    SPIwrite({ 0x41, 0x00, 0x80 });
    
    // config2 register
    SPIwrite({ 0x42, 0x00, 0xD4 });

    // config3 register
    SPIwrite({ 0x43, 0x00, 0xE0 });
  }
  SPI.endTransaction();

  delay(300);

  m_Mode = CA2D::ModeType::TRIGGERED;
}

CA2D::DataType CA2D::getData() {
  // init return value with current head state
  CA2D::DataType data(m_pHead->getState());

  if (getMode() != ModeType::TRIGGERED) return data;

  SPI.beginTransaction(g_settings);

  digitalWrite(CS.A2D, LOW);   // Sets PIN_CHIP_SELECT to low for read
  delayMicroseconds(5);
  SPI.transfer(0x12);   // RDATA command

  data = readData();                     // Resets PIN_CHIP_SELECT to high inside readData()

  SPI.endTransaction();

  return data;
}

