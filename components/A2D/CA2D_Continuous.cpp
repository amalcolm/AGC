#include "CA2D.h"
#include "Setup.h" 
#include "CTimer.h"
#include "Hardware.h"
#include "Config.h"

volatile bool    CA2D::s_dmaActive       = false;
  EventResponder CA2D::s_spiEvent{};
   alignas(32) uint8_t m_rxBuffer[32];
   alignas(32) uint8_t m_txBuffer[32];
   alignas(32) uint8_t m_frBuffer[32];

void CA2D::setMode_Continuous() {

  pinMode(CS.A2D, OUTPUT);
  digitalWrite(CS.A2D, HIGH);

  uint8_t cfg1 = getConfig1();

  SPI.begin();
  SPI.beginTransaction(spiSettings);
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
    digitalWrite(CS.A2D, HIGH);


    // 4) Config: 2 kSPS, reserved bits correct, no CLK out
    SPIwrite({0x41, 0x00, cfg1});     // CONFIG1 = 0xD4 for 1 kSPS);  0xD6 = 250SPS, 0xD5 = 500SPS, OxD4 = 1kSPS, 0xD3 = 2kSPS, ... D0 = 16kSPS; 
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

  NVIC_SET_PRIORITY(IRQ_GPIO1_0_15, 32);

  // MCU side
  pinMode(m_pinDataReady, INPUT); // no pullups; ADS drives the line
  attachInterrupt(digitalPinToInterrupt(m_pinDataReady), CA2D::ISR_Data, FALLING);

  s_spiEvent.attach(onSpiDmaComplete);


  m_pBlockToFill = &m_BlockA;
  m_pBlockToSend = &m_BlockB;
  m_BlockA.clear();
  m_BlockB.clear();

  m_Mode = ModeType::CONTINUOUS;
  USB.printf("A2D: Continuous mode (@%d)", CFG::A2D_SAMPLING_SPEED_Hz);
}

//CTeleCounter TC_ISR{TeleGroup::A2D, 0x40};
//CTeleTimer TT_A2DRead{TeleGroup::A2D, 0x41};
void CA2D::ISR_Data() {// TC_ISR.increment();
   if (Singleton->m_ReadState == ReadState::IDLE) return;
   Singleton->m_dataReady = true;
}


bool CA2D::poll_Continuous() { 

  if (!m_dataReady) { yield(); return false; }

  m_dataReady = false;
  
  if (m_ReadState == ReadState::IGNORE) return false;

  DataType data(Head.getState());  // sets timestamp and stateTime

  if (!s_dmaActive) {
    s_dmaActive = true;
//  TT_A2DRead.start();

    SPI.beginTransaction(spiSettings);
    digitalWriteFast(CS.A2D, LOW);

    arm_dcache_flush(m_txBuffer, sizeof(m_txBuffer));
    arm_dcache_delete(m_rxBuffer, sizeof(m_rxBuffer));
    SPI.transfer(m_txBuffer, m_rxBuffer, 32, s_spiEvent);

    setDebugData(data);

    while (s_dmaActive) yield(); // wait for DMA complete

    dataFromFrame(m_frBuffer, data);

    m_pBlockToFill->tryAdd(data);

  }

  return true;
}

void CA2D::onSpiDmaComplete(EventResponderRef)
{
//  TT_A2DRead.stop();

    arm_dcache_delete( m_rxBuffer, sizeof(m_rxBuffer));
    memcpy(m_frBuffer, m_rxBuffer, sizeof(m_rxBuffer));

    digitalWriteFast(CS.A2D, HIGH);
    SPI.endTransaction();

    s_dmaActive = false;
}

bool errorOutput = false;
void CA2D::setRead(bool enable)
{
  if (m_Mode != ModeType::CONTINUOUS) {  if (!errorOutput) { USB.printf("*** A2D: Cannot setRead() when not in continuous mode."); errorOutput = true; } return; }

  if (enable) {
    delayMicroseconds(50);
    attachInterrupt(digitalPinToInterrupt(m_pinDataReady), CA2D::ISR_Data, FALLING);
  } else {
    detachInterrupt(digitalPinToInterrupt(m_pinDataReady));
    while (s_dmaActive) yield(); // wait for any active DMA to finish
    delayMicroseconds(10);
  }
}
