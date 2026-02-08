#include "CA2D.h"
#include "Setup.h"
#include "CUSB.h"

void CA2D::setMode_Continuous() {

  uint8_t cfg1 = getConfig1();

  SPI.begin();  // ensure SPI is initialized
  delay(2); // let SPI/rails settle

  SPI.beginTransaction(spiSettings);
  {
    // 1) Reset & pause
    SPIwrite({0x06});                 // RESET
    delayMicroseconds(50);            // >18 tCLK

    // 3) Stop RDATAC so we can write regs
    SPIwrite({0x11});                 // SDATAC

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
    SPIwrite({0x08});                 // START (START pin must be held low on the board)
    SPIwrite({0x10});                 // RDATAC
  }
  SPI.endTransaction();


  m_Mode = ModeType::CONTINUOUS;
  USB.printf("A2D: Continuous mode (@%d)", CFG::A2D_SAMPLING_SPEED_Hz);
}



