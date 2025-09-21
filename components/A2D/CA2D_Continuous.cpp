#include "CA2D.h"
#include "Setup.h"
#include "CUSB.h"

// only visible inside Continuous codebase
static CA2D* Singleton = NULL;



void CA2D::setMode_Continuous() {
  uint8_t id = 0;
  if (Singleton) { Serial.print("*** A2D: Single continuous instance only."); return; }
  Singleton = this;

  digitalWrite(CS.A2D, HIGH);

  SPI.begin();
  SPI.beginTransaction(CA2D::g_settings);
  {
    // 1) Stop RDATAC so we can write regs
    SPIwrite({0x11});                 // SDATAC

    // 2) Reset & pause
    SPIwrite({0x06});                 // RESET
    delayMicroseconds(50);            // >18 tCLK

    // 3) (Optional) Read ID to prove SPI link (RREG 0x00 one reg)
    digitalWrite(CS.A2D, LOW);
    SPI.transfer(0x20);               // RREG addr=0x00
    SPI.transfer(0x00);               // read 1 register
    id = SPI.transfer(0x00);
    digitalWrite(CS.A2D, HIGH);
    // check 'id' != 0xFF/0x00

    // 4) Config: 250SPS & clock out enabled for scope
    SPIwrite({0x41, 0x00, 0xB6});     // CONFIG1 = 0xB4 (DR=110, CLK_EN=1)
    //    CONFIG2: test off first (0xC0). If needed, set INT_CAL later.
    SPIwrite({0x42, 0x00, 0xC0});     // CONFIG2

    //    Power up CH1 in normal mode @gain=24. Others can be powered down.
    SPIwrite({0x45, 0x07,
              0x60,                   // CH1SET
              0x81,0x81,0x81,0x81,0x81,0x81,0x81  // CH2..CH8 PD=1
    });

    // 5) Start conversions, then enable RDATAC
    SPIwrite({0x08});                 // START (START pin held low)
    SPIwrite({0x10});                 // RDATAC
  }
  SPI.endTransaction();
  
  // MCU side
  pinMode(m_pinDataReady, INPUT); // no pullups; ADS drives the line
  attachInterrupt(digitalPinToInterrupt(m_pinDataReady), CA2D::ISR_Data, FALLING);

  m_pBlockToFill = &m_BlockA;
  m_pBlockToSend = &m_BlockB;
  m_BlockA.data->clear();
  m_BlockB.data->clear();
  isBlockReadyToSend = false;

  m_Mode = CA2D::ModeType::CONTINUOUS;
  Serial.print("A2D: Continuous mode (id=");
  Serial.print(id);
  Serial.println(")");
}


void CA2D::ISR_Data() { Singleton->parseData(); }

void CA2D::parseData() {
  
  SPI.beginTransaction(CA2D::g_settings);
//{
      digitalWrite(CS.A2D, LOW);
      CA2D::DataType data = readData();
      digitalWrite(CS.A2D, HIGH);
//}
  SPI.endTransaction();
   
  switch (data.State) {
    case CHead::DIRTY: Serial.print('d'); return;
    case CHead::UNSET: Serial.print('u'); return;
    default:       
        if ((data.State & 0x1000) == 0) {Serial.print('.'); break;}  // good data

        Serial.print("\r\nA2D err:");
        Serial.print(data.State & 0xFFF, HEX);
        return;
  }
  
  m_pBlockToFill->data->push_back(data);

  // when full, swap & flag
  if (m_pBlockToFill->data->size() >= CA2D::BlockType::DEBUG_BLOCKSIZE) {
      std::swap(m_pBlockToSend, m_pBlockToFill);

      m_pBlockToFill->data->clear();
      m_pBlockToFill->timeStamp = millis();

      isBlockReadyToSend = true;
      if (m_fnCallback) m_fnCallback(m_pBlockToSend);
  }
/*
  if (data.State != m_State) {
    
    if (m_State != CHead::DIRTY) {
      std::swap(m_pBlockToSend, m_pBlockToFill);

      m_pBlockToFill->data->clear();
      m_pBlockToFill->timeStamp = millis();

      m_pBlockToSend->State = m_State;
      isBlockReadyToSend = true;
    }

    m_State = data.State;
  }
  m_pBlockToFill->data->push_back(data);   
*/


}

