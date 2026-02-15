#include "CA2D.h"
#include "Setup.h"
#include "CUSB.h"

void CA2D::setMode(ModeType mode) {

  uint8_t cfg1 = getConfig1();
  uint8_t id = 0;

  SPI.beginTransaction(spiSettings);
  {
    // 1) Make sure we're not in RDATAC (so writes are allowed)
    SPIwrite({ 0x11 }); // SDATAC
    delayMicroseconds(5);

    // 2) Reset & pause
    SPIwrite({0x06});                 // RESET
    delayMicroseconds(50);            // >18 tCLK

    // 3) (Optional sanity: read ID here if you have a readReg helper)
    digitalWrite(CS.A2D, LOW);
    delayMicroseconds(4);
           SPI.transfer(0x20);               // RREG addr=0x00
           SPI.transfer(0x00);               // read 1 register
      id = SPI.transfer(0x00);  // clock out the register data
    digitalWrite(CS.A2D, HIGH);

    // 4) Config: 2 kSPS, reserved bits correct, no CLK out
    // bit7 must be 1 per datasheet; reserved pattern honored;
    SPIwrite({ 0x41, 0x00, cfg1 });     // CONFIG1 = 0xD4 for 1 kSPS);  0xD6 = 250SPS, 0xD5 = 500SPS, OxD4 = 1kSPS, 0xD3 = 2kSPS, ... D0 = 16kSPS; 
                                      // bits: 1 DAISY_EN=1 CLK_EN=0 1 0 DR=100 (1 kSPS)


    SPIwrite({ 0x42, 0x00, 0xC0 });     // CONFIG2 (baseline; no internal test)


    SPIwrite({ 0x43, 0x00, 0xE0 });     // CONFIG3 (enable internal reference buffer)

    // 5) channels: CH1 normal input, gain=1; others powered-down & shorted
    SPIwrite({ 0x45, 0x07,
               0x00,                 // CH1SET: PD=0, GAIN=000 (x1), SRB2=0, MUX=000 (normal)
               0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81  // CH2..CH8: PD=1, MUX=short
    });

    // 6) Start conversions, then enable RDATAC
    SPIwrite({ 0x08 });                 // START (START pin must be held low on the board)

    if (mode == ModeType::CONTINUOUS) 
      SPIwrite({ 0x10 });                 // RDATAC
    
  }
  SPI.endTransaction();

  if ((id & 0x1F) != 0x1E) USB.printf(" - Warning: unexpected ID 0x%02X", id);

  m_Mode = mode;

  switch (mode) {
    case ModeType::TRIGGERED : USB.printf("A2D: Triggered mode"); 
      break;
    case ModeType::CONTINUOUS: USB.printf("A2D: Continuous mode (@%d)", CFG::A2D_SAMPLING_SPEED_Hz);
      break;
      
    default:                   USB.printf("A2D: Mode set to UNSET or unknown value");
      break;
  }


}



