#include "CA2D.h"
#include "Setup.h"
#include "CHead.h"
#include "Helpers.h"
#include "Hardware.h"
#include "CTimer.h"
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

CA2D& CA2D::begin() {
  setMode(m_Mode);
  return *this;
}


void CA2D::setMode(CA2D::ModeType mode) {

  pinMode(PIN_SPI_SCK   , OUTPUT); // SPI SCK
  pinMode(PIN_SPI_MOSI  , OUTPUT); // SPI MOSI
  pinMode(PIN_SPI_MISO  , INPUT ); // SPI MISO
  pinMode(CS.A2D        , OUTPUT); // SPI CS
  pinMode(m_pinDataReady, INPUT ); // no pullups; ADS drives the line

  switch (mode) {
    case CA2D::ModeType::CONTINUOUS: setMode_Continuous();  break;
    case CA2D::ModeType::TRIGGERED : setMode_Triggered ();  break;
    default:  break;
  }

  m_pBlockToFill = &m_BlockA;
  m_pBlockToSend = &m_BlockB;
  m_BlockA.clear();
  m_BlockB.clear();

}

bool CA2D::poll() {
  double start = Timer.getStateTime();
  bool newData = (m_Mode == ModeType::CONTINUOUS) ? poll_Continuous() : poll_Triggered();
  double end = Timer.getStateTime();

  Timer.setPollDuration(end - start);

  if (newData) {
    Timer.addEvent(EventKind::A2D_READ_START   , start);
    Timer.addEvent(EventKind::A2D_READ_COMPLETE, end  );  
  }
  return newData;
}

// Call with CS already LOW (continuous). Return false if header not found.
bool CA2D::readFrame(uint8_t (&raw)[32]) {
  
  // Read status first
  for (int i=0;i<3;i++) raw[i] = SPI.transfer(0x00);

  // Header check per datasheet: top nibble of first status byte = 1100b (0xC?)
  if ((raw[0] & 0xF0) != 0xC0) {
    // Drain remaining bytes for this bad frame so we realign next time
    for (int i=3;i<27;i++) (void)SPI.transfer(0x00);
    LED.RED5.set();
    return false;
  }

  // Read 8 channels × 3 bytes
  for (int i=3;i<27;i++) raw[i] = SPI.transfer(0x00);

  bool isZero = (raw[3] == 0 && raw[4] == 0 && raw[5] == 0);
  
  if (isZero) {
    LED.RED6.set();
    return false;
  }
  LED.RED6.clear(); 
  return true;
}




// 24-bit sign-extend (two’s-complement)
inline int32_t be24_to_s32(const uint8_t b2, const uint8_t b1, const uint8_t b0) {
  int32_t v = (int32_t(b2) << 16) | (int32_t(b1) << 8) | int32_t(b0);
  if (v & 0x00800000) v |= 0xFF000000; // sign-extend bit 23
  return v;
}

void CA2D::dataFromFrame(uint8_t (&raw)[32], DataType& data) {

  const uint8_t* p = &raw[3]; // skip status
  for (int ch=0; ch<8; ++ch) {
    int32_t val = be24_to_s32(p[0], p[1], p[2]);
    p += 3;
    data.channels[ch] = val;
  }
}

DataType CA2D::readData() {

  DataType data(Head.getState());

  uint8_t raw[32];
  bool ok = readFrame(raw);
  if (ok) 
    dataFromFrame(raw, data);
  else {
    data.state = DIRTY;
    return data;
  }

  setDebugData(data);

  return data;
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
      (analogRead(offsetPot1.getSensorPin()) << 16) |
      (analogRead(offsetPot2.getSensorPin())      );
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
