#include "CA2D.h"
#include "Setup.h"

// only visible inside Continuous codebase
static CA2D* Singleton = NULL;


void CA2D::setMode_Continuous() {
  if (Singleton) { Serial.print("*** A2D: Single continuous instance only."); return; }
  Singleton = this;

  pinMode(CS.A2D, OUTPUT);

  digitalWrite(CS.A2D, HIGH);

  SPI.begin();

  SPI.beginTransaction(g_settings);
  {
    SPIwrite( { 0x11 } );  // SDATAC
    //                        0     1     2     3     4     5     6     7     8   // which channels to enable
    SPIwrite( { 0x45, 0x07, 0x00, 0x00, 0x01, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81 } );
   
    SPIwrite( { 0x41, 0x00, 0x80 } ); // Config1 - 16k samples per second
    SPIwrite( { 0x42, 0x00, 0xD4 } );
    SPIwrite( { 0x43, 0x00, 0xE0 } );
  }
  SPI.endTransaction();
  
  // --- Start Continuous Conversion ---
  SPI.beginTransaction(g_settings);
  {
    SPIwrite( { 0x08 } );  // START
    SPIwrite( { 0x10 } ); // RDATAC
  }
  SPI.endTransaction();


  m_pBlockToFill = &m_BlockA;
  m_pBlockToSend = &m_BlockB;
  m_State        = CHead::DIRTY;

  delay(300);  

  // Attach the DRDY interrupt (trigger on falling edge).
  attachInterrupt(digitalPinToInterrupt(m_pinDataReady.getNum()), CA2D::ISR_Data, FALLING);

  m_Mode = CA2D::ModeType::CONTINUOUS;
}


void CA2D::ISR_Data() { Singleton->parseData(); }

void CA2D::parseData() {
  CA2D::DataType data = readData();   if (data.State == CHead::DIRTY) return;

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
}

volatile CA2D::BlockType* CA2D::getBlockToSend() { return m_pBlockToSend; }

void CA2D::releaseBlockToSend() {
   m_pBlockToSend->data->clear(); 
   isBlockReadyToSend = false;
}
