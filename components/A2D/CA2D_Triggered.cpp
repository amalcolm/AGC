#include "CA2D.h"
#include "Setup.h"
#include "CHead.h"
#include "CTimer.h"
#include "Config.h"

void CA2D::setMode_Triggered()
{
  uint8_t cfg1 = getConfig1();

  SPI.begin();
  delay(2); // let SPI/rails settle

  SPI.beginTransaction(spiSettings);
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
    SPIwrite({ 0x41, 0x00, cfg1 }); // WREG @0x01, write 1 byte (0x96)

    // 4) CONFIG2: baseline with reserved bits correct; no internal test source
    // If you want the internal test generator later, change 0xC0 -> 0xD0 and set CHnSET MUX accordingly.
    SPIwrite({ 0x42, 0x00, 0xC0 }); // WREG @0x02

    // 5) CONFIG3: internal reference buffer enabled; bias defaults (tweak later if needed)
    SPIwrite({ 0x43, 0x00, 0xE0 }); // WREG @0x03

    // channels: CH1 normal input, gain=1; others powered-down & shorted
    SPIwrite({ 0x45, 0x07,
               0x00,                 // CH1SET: PD=0, GAIN=000 (x1), SRB2=0, MUX=000 (normal)
               0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81  // CH2..CH8: PD=1, MUX=short
    });

    // 7) START conversions so DRDY will pulse and RDATA will have fresh samples
    SPIwrite({ 0x08 }); // START
    delayMicroseconds(5);
  }
  SPI.endTransaction();

  attachInterrupt(digitalPinToInterrupt(m_pinDataReady), ISR_Mark, FALLING);

  m_dataReady = false;

  m_pBlockToFill = &m_BlockA;
  m_pBlockToSend = &m_BlockB;
  m_pBlockToFill->clear();

  delay(300);

  USB.printf("A2D: Triggered mode");

  m_Mode = CA2D::ModeType::TRIGGERED;
}

void CA2D::ISR_Mark() { Singleton->m_dataTime = Timer.elapsed(); }

DataType CA2D::getData() {

  DataType data(Head.getState());   if (getMode() != ModeType::TRIGGERED) return data;
  

  SPI.beginTransaction(spiSettings);
  {
    digitalWrite(CS.A2D, LOW);
    SPI.transfer(0x12);   // RDATA command

    data = readData();

    digitalWrite(CS.A2D, HIGH);
    delayMicroseconds(200);

  }
  SPI.endTransaction();

  return data;
}

const uint64_t PERIOD_TICKS = static_cast<uint64_t>(
  std::round(1.0 / (CTimer::getSecondsPerTick() * CFG::READING_SPEED_Hz))  // Fixed rate testing...
);


void CA2D::setNextReadTime(uint64_t time) {
  m_nextReadTime = time + PERIOD_TICKS * 2/6;
} 

bool CA2D::poll_Triggered() {
  
  uint64_t now = Timer.elapsed();

  if (now < m_nextReadTime || m_ReadState == ReadState::IDLE) {
      yield();
      return false;
  }

  m_nextReadTime = (m_nextReadTime == 0) ? now + PERIOD_TICKS : m_nextReadTime + PERIOD_TICKS;
 
  // Fetch the fresh data (RDATA grabs whatever's latest at this moment)
  DataType data = getData();
  m_pBlockToFill->tryAdd(data);

  return true;
}

void CA2D::readThenStore() {
  DataType data = getData();
  m_pBlockToFill->tryAdd(data);
}