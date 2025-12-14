#include "CA2D.h"
#include "Setup.h"
#include "CHead.h"
#include "Hardware.h"

void CA2D::setMode_Triggered()
{
  SPI.begin();
  delay(2); // let SPI/rails settle

  SPI.beginTransaction(Hardware::SPIsettings);
  {
    // Configuration sequence per ADS1299 datasheet
    // 1) Make sure we're not in RDATAC (so writes are allowed)
    SPIwrite({ 0x11 }); // SDATAC
    delayMicroseconds(5);

    // 2) Clean reset (covers power-up races); wait a bit afterwards
    SPIwrite({ 0x06 }); // RESET
    delay(2);

    // (Optional sanity: read ID here if you have a readReg helper)
    digitalWrite(CS.A2D, LOW);
    SPI.transfer(0x20);               // RREG addr=0x00
    SPI.transfer(0x00);               // read 1 register
    digitalWrite(CS.A2D, HIGH);

    // 3) CONFIG1: 0xD4 = 0b11010100
    // bit7 must be 1 per datasheet; reserved pattern honored; DR=100 (1000SPS)
    SPIwrite({ 0x41, 0x00, 0xD4 }); // WREG @0x01, write 1 byte (0x96)

    // 4) CONFIG2: baseline with reserved bits correct; no internal test source
    // If you want the internal test generator later, change 0xC0 -> 0xD0 and set CHnSET MUX accordingly.
    SPIwrite({ 0x42, 0x00, 0xC0 }); // WREG @0x02

    // 5) CONFIG3: internal reference buffer enabled; bias defaults (tweak later if needed)
    SPIwrite({ 0x43, 0x00, 0xE0 }); // WREG @0x03

    // 6) channels: CH1 normal electrode, gain=1 (0x00). Others shorted for quiet baseline (0x61).
    //    Later, switch each CHn to 0x00 when wiring electrodes.
    SPIwrite({
        0x45, 0x07, // WREG @0x05, count=8 regs
        0x00,       // CH1SET: normal input, gain=1
        0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61 // CH2..CH8: input short, gain=24
    });

    // (Optional) Write SRB/Bias registers here if you use them

    // 7) START conversions so DRDY will pulse and RDATA will have fresh samples
    SPIwrite({ 0x08 }); // START
    delayMicroseconds(5);
  }
  SPI.endTransaction();

  // In triggered mode we do NOT send RDATAC; your getData() will:
  //   wait for DRDY low -> CS low -> 0x12 (RDATA) -> read 3+24 bytes -> CS high.

  m_dataReady = false;

  m_pBlockToFill = &m_BlockA;
  m_pBlockToSend = &m_BlockB;
  m_pBlockToFill->clear();

  delay(300);

  USB.printf("A2D: Triggered mode");

  m_Mode = CA2D::ModeType::TRIGGERED;
}

DataType CA2D::getData() {

  DataType data(Head.getState());   if (getMode() != ModeType::TRIGGERED) return data;
  

  SPI.beginTransaction(Hardware::SPIsettings);
  {
    digitalWrite(CS.A2D, LOW);
    delayMicroseconds(5);
    SPI.transfer(0x12);   // RDATA command

    data = readData();

    digitalWrite(CS.A2D, HIGH);

  }
  SPI.endTransaction();

  return data;
}

