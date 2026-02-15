#include "CA2D.h"
#include "Setup.h"
#include "CHead.h"
#include "Helpers.h"
#include "Hardware.h"
#include "CA2DTimer.h"
#include "Config.h"

CA2D* CA2D::Singleton = nullptr;

const std::array<std::pair<uint32_t, uint8_t>, 8> CA2D::SpeedLookup = {{
    {16000, 0x90}, { 8000, 0x91}, { 4000, 0x92}, { 2000, 0x93},
    { 1000, 0x94}, {  500, 0x95}, {  250, 0x96}, {  125, 0x97}
}};

CA2D::CA2D() {
  m_Mode = CFG::A2D_USE_TRIGGERED_MODE ? ModeType::TRIGGERED : ModeType::CONTINUOUS;
  
  if (Singleton) { USB.printf("*** A2D: Single continuous instance only."); return; }
  Singleton = this;
}

void CA2D::begin() {
  pinMode(CS.A2D        , OUTPUT); // SPI CS
  pinMode(m_pinDataReady, INPUT ); // no pullups; ADS drives the line

  setMode(m_Mode);

  // Set up DMA event handler
  s_spiEvent.attach(onSpiDmaComplete);

  // We use two blocks and swap between them, allowing reading into one while the other is being sent
  m_pBlockToFill = &m_BlockA;
  m_pBlockToSend = &m_BlockB;
  m_BlockA.clear();
  m_BlockB.clear();

  NVIC_SET_PRIORITY(IRQ_GPIO1_0_15, 32);  // raise priority of GPIO1 interrupts

  // attach the dataReadyPin to the interrupt handler, fires on falling edge (when ADS has data ready)
  attachInterrupt(digitalPinToInterrupt(m_pinDataReady), CA2D::ISR_Data, FALLING);
}

void CA2D::ISR_Data() {

  uint32_t now = ARM_DWT_CYCCNT; 
  if (Singleton->m_ReadState != ReadState::IDLE) {
    Singleton->m_dataStateTime = Timer.getStateTime(now);
    Singleton->m_dataReady = true;
  }
  
  uint32_t duration = now - Timer.A2D.getLastMarker(); 
  Timer.A2D.reset(now, duration);
}


void CA2D::setMode(CA2D::ModeType mode) {
  switch (mode) {
    case CA2D::ModeType::CONTINUOUS: setMode_Continuous();  break;
    case CA2D::ModeType::TRIGGERED : setMode_Triggered ();  break;
    default:  break;
  }
}

bool CA2D::poll() {
  double start = Timer.getStateTime();

  if (!m_dataReady) { yield(); return false; }

  m_dataReady = false;
  Timer.addEvent(EventKind::A2D_DATA_READY, m_dataStateTime);



  DataType data = getData_DMA();

  double end = Timer.getStateTime();

  Timer.setPollDuration(end - start);

  Timer.addEvent(EventKind::A2D_READ_START   , start);
  Timer.addEvent(EventKind::A2D_READ_COMPLETE, end  );  

  if (m_ReadState != ReadState::IGNORE) 
    m_pBlockToFill->tryAdd(data);

  return data.state != DIRTY;
}



void CA2D::setDebugData(DataType& data) {
  static uint8_t sequenceNumber = 0;
 
  auto& [state, offsetPot1, offsetPot2, gainPot] = getHWforState(data);
  
  data.hardwareState = 
      (sequenceNumber++      << 24) |
      (offsetPot1.getLevel() << 16) |
      (offsetPot2.getLevel() <<  8) |
      (gainPot   .getLevel()      );

  data.sensorState =
      (offsetPot1.lastSensorValue() << 16) |
      (offsetPot2.lastSensorValue()      ); 
//      (analogRead(offsetPot1.getSensorPin()) << 16) |
//      (analogRead(offsetPot2.getSensorPin())      );
}


uint8_t CA2D::getConfig1() const {
  uint8_t config1 = 0x94;

  // Set speed bits based on SAMPLING_SPEED
  for (const auto& [speed, code] : CA2D::SpeedLookup) {
    if (CFG::A2D_SAMPLING_SPEED_Hz == speed) {
      config1 = code;
      break;
    }
  }

  return config1;
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





void CA2D::setBlockState(StateType state) {
  m_pBlockToFill->state = state;
  noInterrupts();
  {
    std::swap(m_pBlockToSend, m_pBlockToFill);
  }
  interrupts();

  m_pBlockToFill->timestamp = Timer.getConnectTime();
  m_pBlockToFill->clear();

  USB.buffer(m_pBlockToSend);

  if (m_fnCallback) m_fnCallback(m_pBlockToSend);
}
