#include "CA2D.h"
#include "Setup.h" 
#include "CUSB.h"

// only visible inside Continuous codebase
static CA2D* Singleton = NULL;


void CA2D::setMode_Continuous() {
  uint8_t id = 0;
  if (Singleton) { Serial.print("*** A2D: Single continuous instance only."); return; }
  Singleton = this;

  pinMode(CS.A2D, OUTPUT);
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

    // 4) Config: 2 kSPS, reserved bits correct, no CLK out
    SPIwrite({0x41, 0x00, 0xD4});     // CONFIG1 = 0xD4 for 1 kSPS); use 0xD3 (2 kSPS)
                                      // bits: 1 DAISY_EN=1 CLK_EN=0 1 0 DR=100 (1 kSPS)
    SPIwrite({0x42, 0x00, 0xC0});     // CONFIG2 (baseline; no internal test)
    SPIwrite({0x43, 0x00, 0xE0});     // CONFIG3 (enable internal reference buffer)

    // channels: CH1 normal input, gain=1; others powered-down & shorted
    SPIwrite({ 0x45, 0x07,
               0x00,                 // CH1SET: PD=0, GAIN=000 (x1), SRB2=0, MUX=000 (normal)
               0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81  // CH2..CH8: PD=1, MUX=short
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
  m_BlockA.clear();
  m_BlockB.clear();

  m_Mode = ModeType::CONTINUOUS;
  Serial.print("A2D: Continuous mode (id=");
  Serial.print(id);
  Serial.println(")");
}

void CA2D::ISR_Data() { Singleton->m_dataReady = true; }

void CA2D::pollData() { 
  
  if (!m_dataReady || getMode() != ModeType::CONTINUOUS) return;

  m_dataReady = false;

  DataType data;

  SPI.beginTransaction(g_settings);
  {
    digitalWrite(CS.A2D, LOW);
    delayMicroseconds(2);

    data = readData();
    digitalWrite(CS.A2D, HIGH);
  }
  SPI.endTransaction();

  m_pBlockToFill->push_back(data);
}

void CA2D::setBlockState(StateType state) {
  m_pBlockToFill->state = state;
  noInterrupts();
  std::swap(m_pBlockToSend, m_pBlockToFill);
  interrupts();

  m_pBlockToFill->timeStamp = millis();

  BlockType* blockToSend = getBlockToSend();
  USB.buffer(blockToSend);

  if (m_fnCallback) m_fnCallback(blockToSend);
  releaseBlockToSend();
}
