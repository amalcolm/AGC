#include "CA2D.h"
#include "Setup.h" 
#include "CTimer.h"
#include "CUSB.h"
#include "Hardware.h"
#include "CHead.h"
#include <vector>
#include <tuple>
// only visible inside Continuous codebase
static CA2D* Singleton = NULL;


void CA2D::setMode_Continuous() {
  static constexpr std::array<std::pair<uint32_t, uint8_t>, 8> speedLookup = {{
      {16000, 0xD0}, { 8000, 0xD1}, { 4000, 0xD2}, { 2000, 0xD3},
      { 1000, 0xD4}, {  500, 0xD5}, {  250, 0xD6}, {  125, 0xD7}
  }};


  if (Singleton) { Serial.print("*** A2D: Single continuous instance only."); return; }
  Singleton = this;

  pinMode(CS.A2D, OUTPUT);
  digitalWrite(CS.A2D, HIGH);

  uint8_t cfg1 = 0xD4; // default 1kSPS
  for (const auto& [speed, value] : speedLookup)
    if (CA2D::SAMPLING_SPEED == speed) {
      cfg1 = value;
      break;
    }


  SPI.begin();
  SPI.beginTransaction(Hardware::SPIsettings);
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

  // MCU side
  pinMode(m_pinDataReady, INPUT); // no pullups; ADS drives the line
  attachInterrupt(digitalPinToInterrupt(m_pinDataReady), CA2D::ISR_Data, FALLING);

  m_pBlockToFill = &m_BlockA;
  m_pBlockToSend = &m_BlockB;
  m_BlockA.clear();
  m_BlockB.clear();

  m_Mode = ModeType::CONTINUOUS;
  Serial.print("A2D: Continuous mode (@");
  Serial.print(CA2D::SAMPLING_SPEED);
  Serial.println("hz)");
}

void CA2D::ISR_Data() {
   if (Head.isReady() == false) return;
   Singleton->m_dataReady = true;
   TeleCount[0]++; 
}
  

bool CA2D::pollData() { 

  if (!m_dataReady) return false;
  m_dataReady = false;

//  uint64_t start = CTimer::time();
  SPI.beginTransaction(Hardware::SPIsettings);
  {
    digitalWrite(CS.A2D, LOW);
    delayMicroseconds(5);

    DataType data = readData();
    
    digitalWrite(CS.A2D, HIGH);
    if (data.channels[0] > 0)
      m_pBlockToFill->push_back(data);

  }
  SPI.endTransaction();
//  uint64_t duration = CTimer::time() - start;

//  Tele(CTelemetry::Group::A2D, CA2D::TeleKind::TIME, 1, duration * CTimer::getMicrosecondsPerTick());
  TeleCount[2]++; 

  return true;
}

void CA2D::setBlockState(StateType state) {
  m_pBlockToFill->state = state;
  noInterrupts();
  std::swap(m_pBlockToSend, m_pBlockToFill);
  interrupts();

  m_pBlockToFill->timeStamp = Timer.getConnectTime();
  m_pBlockToFill->clear();

  USB.buffer(m_pBlockToSend);

  if (m_fnCallback) m_fnCallback(m_pBlockToSend);
}
