#include "CA2D.h"
#include "Setup.h"
#include "CHead.h"

void CA2D::setMode_Triggered()
{
  uint8_t cfg1 = getConfig1();
  uint8_t id = 0;

  SPI.beginTransaction(spiSettings);
  {
    // 1) Make sure we're not in RDATAC (so writes are allowed)
    SPIwrite({ 0x11 }); // SDATAC
    delayMicroseconds(5);

    // 2) Clean reset (covers power-up races); wait a bit afterwards
    SPIwrite({ 0x06 }); // RESET
    delayMicroseconds(50); // >18 tCLK per datasheet

    // (Optional sanity: read ID here if you have a readReg helper)
    digitalWrite(CS.A2D, LOW);
    delayMicroseconds(4);
           SPI.transfer(0x20);               // RREG addr=0x00
           SPI.transfer(0x00);               // read 1 register
      id = SPI.transfer(0x00);  // clock out the register data
    digitalWrite(CS.A2D, HIGH);

    // 3) CONFIG1: 0xD4 = 0b11010{DR}  e.g. DR=100 for 1 kSPS; 
    // bit7 must be 1 per datasheet; reserved pattern honored;
    SPIwrite({ 0x41, 0x00, cfg1 }); // WREG @0x01, write 1 byte (0x96)

    // 4) CONFIG2: baseline with reserved bits correct; no internal test source
    // If you want the internal test generator later, change 0xC0 -> 0xD0 and set CHnSET MUX accordingly.
    SPIwrite({ 0x42, 0x00, 0xC0 }); // WREG @0x02

    // 5) CONFIG3: internal reference buffer enabled; bias defaults
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

  m_dataReady = false;

  USB.printf("A2D: Triggered mode"); 
    if ((id & 0x1F) != 0x1E) USB.printf(" - Warning: unexpected ID 0x%02X", id);
  m_Mode = CA2D::ModeType::TRIGGERED;
}



DataType CA2D::getData() {

  DataType data(Head.getState());   if (getMode() != ModeType::TRIGGERED) return data;
  

  SPI.beginTransaction(spiSettings);
  {
    digitalWrite(CS.A2D, LOW);
    SPI.transfer(0x12);   // RDATA command

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


    digitalWrite(CS.A2D, HIGH);
    delayMicroseconds(200);

  }
  SPI.endTransaction();

  return data;
}



// Call with CS already LOW (continuous). Return false if header not found.
bool CA2D::readFrame(uint8_t (&raw)[32]) {
  
  // Read status first
  for (int i=0;i<3;i++) raw[i] = SPI.transfer(0x00);

  // Header check per datasheet: top nibble of first status byte = 1100b (0xC?)
  if ((raw[0] & 0xF0) != 0xC0) {
    // Drain remaining bytes for this bad frame so we realign next time
    for (int i=3;i<27;i++) (void)SPI.transfer(0x00);
    LED.set(5);
    return false;
  }

  // Read 8 channels × 3 bytes
  for (int i=3;i<27;i++) raw[i] = SPI.transfer(0x00);

  bool isZero = (raw[3] == 0 && raw[4] == 0 && raw[5] == 0);
  
  if (isZero) {
    LED.set(6);
    return false;
  }
  LED.clear(6); 
  return true;
}




// 24-bit sign-extend (two’s-complement)
inline int32_t be24_to_s32(const uint8_t b2, const uint8_t b1, const uint8_t b0) {
  int32_t v = (int32_t(b2) << 16) | (int32_t(b1) << 8) | int32_t(b0);
  if (v & 0x00800000) v |= 0xFF000000; // sign-extend bit 23
  return v;
}

// Extract channel data from raw frame bytes into DataType structure
void CA2D::dataFromFrame(uint8_t (&raw)[32], DataType& data) {
  const uint8_t* p = &raw[3]; // skip status
  for (int ch=0; ch<8; ++ch) {
    int32_t val = be24_to_s32(p[0], p[1], p[2]);
    p += 3;
    data.channels[ch] = val;
  }
}

