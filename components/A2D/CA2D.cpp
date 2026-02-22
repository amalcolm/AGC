#include "CA2D.h"
#include "Setup.h"
#include "CHead.h"
#include "Helpers.h"
#include "Hardware.h"
#include "CA2DTimer.h"
#include "Config.h"

CA2D* CA2D::Singleton = nullptr;

const std::array<std::pair<uint32_t, uint8_t>, 8> SpeedLookup = {{
    {16000, 0x90}, { 8000, 0x91}, { 4000, 0x92}, { 2000, 0x93},
    { 1000, 0x94}, {  500, 0x95}, {  250, 0x96}, {  125, 0x97}
}};

CA2D::CA2D() {
  m_mode = CFG::A2D_USE_CONTINUOUS_MODE ? ModeType::CONTINUOUS : ModeType::TRIGGERED;
  
  if (Singleton) { USB.printf("*** A2D: Single continuous instance only."); return; }
  Singleton = this;
}

void CA2D::begin() {
  pinMode(CS.A2D        , OUTPUT); // SPI CS
  pinMode(m_pinDataReady, INPUT ); // no pullups; ADS drives the line

  // Configure the ADS1299, and sent START command, and RDATAC if in continuous mode
  configure_ADS1299();

  init_DMA();

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

  Singleton->m_dataStateTime = Timer.getStateTime(now);
  Singleton->m_dataReady = true;
  
  Timer.A2D.setDataReady(now);
}



void CA2D::waitForNextDataReady() const {
  while (!m_dataReady) {
    yield();
  }
}


bool CA2D::poll() {
  double start = Timer.getStateTime();

  switch (m_mode) {
    case ModeType::CONTINUOUS: if (!m_dataReady       ) { yield(); return false; }       break;
    case ModeType::TRIGGERED:  if (Timer.A2D.waiting()) { yield(); return false; }       break;
    default: return false;
  }
  m_dataReady = false;  // reset flag

  bool result = true;


  if (m_ReadState != ReadState::IDLE) {
     DataType data = getData();
     if (m_mode == ModeType::CONTINUOUS) data.stateTime = m_dataStateTime;
     m_pBlockToFill->tryAdd(data);
     result = data.state != DIRTY;
  }

  double end = Timer.getStateTime();
  Timer.updateMaxPollDuration(end - start);

  Timer.addEvent(EventKind::A2D_READ_START   , start);
  Timer.addEvent(EventKind::A2D_READ_COMPLETE, end  );  

  return result;
}




void CA2D::setDebugData(DataType& data) {
  static uint8_t sequenceNumber = 0;
 
  auto& [state, offset1_hi, offset1_lo, offsetPot1, offsetPot2, gainPot, _] = getHWforState(data);
  
  uint32_t hi32 =
      (offsetPot1.getLevel() << 24) 
    | (offset1_hi.getLevel() << 16)
    | (offset1_lo.getLevel() <<  8)
    | (++sequenceNumber & 0xFF); 

  uint32_t lo32 = 
      (offsetPot2.getLevel() << 24)
    | (gainPot   .getLevel() << 16)
    | (0xFFFF);

  data.hardwareState = (uint64_t(hi32) << 32) | uint64_t(lo32);;

  data.sensorState =
      (offsetPot1.lastSensorValue() << 16)
    | (offsetPot2.lastSensorValue()      );
}


uint8_t CA2D::getConfig1() const {
  uint8_t config1 = 0x94;

  // Set speed bits based on SAMPLING_SPEED
  for (const auto& [speed, code] : SpeedLookup) {
    if (CFG::A2D_SAMPLING_SPEED_Hz == speed) {
      config1 = code;
      break;
    }
  }

  return config1;
}


void CA2D::SPIwrite(std::initializer_list<uint8_t> data) {
  static C32bitTimer spiTimer = C32bitTimer::From_uS(2);

  digitalWrite(CS.A2D, LOW);
  delayMicroseconds(5);
  for (uint8_t b : data) {
    spiTimer.wait();
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
