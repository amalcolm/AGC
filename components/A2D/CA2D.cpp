#include "CA2D.h"
#include "Setup.h"
#include "CHead.h"
#include "Helpers.h"
#include "Hardware.h"
#include "CA2DTimer.h"
#include "Config.h"

const std::array<std::pair<uint32_t, uint8_t>, 8> SpeedLookup = {{
    {16000, 0x90}, { 8000, 0x91}, { 4000, 0x92}, { 2000, 0x93},
    { 1000, 0x94}, {  500, 0x95}, {  250, 0x96}, {  125, 0x97}
}};

CA2D::CA2D() {
  m_mode = CFG::A2D_USE_CONTINUOUS_MODE ? ModeType::CONTINUOUS : ModeType::TRIGGERED;
  m_ReadState = ReadState::IDLE;
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

  NVIC_SET_PRIORITY(IRQ_GPIO2_0_15, 1);  // raise priority of GPIO1 interrupts

  // attach the dataReadyPin to the interrupt handler, fires on falling edge (when ADS has data ready)
  attachInterrupt(digitalPinToInterrupt(m_pinDataReady), CA2D::ISR_Data, FALLING);
}

CTeleCounter TC_Interrupts{TeleGroup::A2D, 0};
CTeleValue   TV_InterruptCount(TeleGroup::A2D, 9);
volatile uint32_t interruptCount = 0;
void CA2D::ISR_Data() {
  uint32_t now = ARM_DWT_CYCCNT; 
  A2D.m_dataStateTime = Timer.getStateTime(now);
  A2D.m_dataReady = true;
  
  Timer.A2D.setDataReady(now);
  TC_Interrupts.increment();
  interruptCount++;
  
  delayMicroseconds(4); // ensure we meet timing requirements for CS hold time and data ready setup time before exiting ISR
}



void CA2D::waitForNextDataReady() {
  while (!m_dataReady)
    yield();

  poll();
}

CTeleCounter TC_Poll{TeleGroup::A2D, 1};
CTeleCounter TC_Read(TeleGroup::A2D, 2);
bool CA2D::poll() {
  double start = Timer.getStateTime();
  TC_Poll.increment();

  switch (m_mode) {
    case ModeType::CONTINUOUS: if (!m_dataReady       ) { TV_InterruptCount.set(interruptCount);  yield(); return false; } break;
    case ModeType::TRIGGERED:  if (Timer.A2D.waiting()) { TV_InterruptCount.set(interruptCount);  yield(); return false; } break;
    default: return false;
  }
  
  m_dataReady = false;  // reset flag
  TC_Read.increment();
  
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
 
  auto& [state, TIA, offsetPot2, gainPot, _] = getHWforState(data);
  
  uint32_t hi32 =
    ((TIA.mid.getLevel() & 0xFFu) << 24) |
    ((TIA.top.getLevel() & 0xFFu) << 16) |
    ((TIA.bot.getLevel() & 0xFFu) <<  8) |
    ((++sequenceNumber)     & 0xFFu);

uint32_t lo32 =
    ((offsetPot2.getLevel() & 0xFFu) << 24) |
    ((gainPot   .getLevel() & 0xFFu) << 16) |
    0xFFFFu;

  data.hardwareState = (uint64_t(hi32) << 32) | uint64_t(lo32);

  data.sensorState =
    (   TIA.mid.lastSensorValue() << 16) |
    (offsetPot2.lastSensorValue()      );
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

  if (data.size() == 0) return;

  digitalWrite(CS.A2D, LOW);
  delayMicroseconds(4);

  // get length from data.end() - data.begin();
  if (data.size() == 1) {
    SPI.transfer(*data.begin());
  } else {
    for (uint8_t b : data) {
      SPI.transfer(b);
      spiTimer.wait();
    }
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
