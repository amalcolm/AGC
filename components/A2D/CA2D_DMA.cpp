#include "CA2D.h"
#include "Setup.h"
#include "CUSB.h"
#include "CHead.h"
#include "Helpers.h"

// buffers for DMA SPI transfers - must be 32-byte aligned for cache management on Teensy 4.x
alignas(32) uint8_t m_rxBuffer[32];
alignas(32) uint8_t m_txBuffer[32];
alignas(32) uint8_t m_frBuffer[32];

DataType CA2D::getData() {

  if (s_dmaActive) return DataType(DIRTY);

  DataType data(Head.getState());  // sets timestamp and stateTime

  s_dmaActive = true;

  SPI.beginTransaction(spiSettings);
  digitalWriteFast(CS.A2D, LOW);

  if (m_mode == ModeType::TRIGGERED) 
    (void)SPI.transfer(0x12); // RDATA command; returned byte is ignored

  arm_dcache_flush(m_txBuffer, sizeof(m_txBuffer));
  arm_dcache_delete(m_rxBuffer, sizeof(m_rxBuffer));

  Timer.addEvent(EventKind::SPI_DMA_START);

  SPI.transfer(m_txBuffer, m_rxBuffer, 32, s_spiEvent);

  setDebugData(data);

  while (s_dmaActive) yield(); // wait for DMA complete (CS raised in callback)

  Timer.addEvent(EventKind::SPI_DMA_COMPLETE);

  bool badHeader = (m_frBuffer[0] & 0xF0) != 0xC0; // status[0] header nibble must be 0xC
  bool isZero = (m_frBuffer[3] == 0 && m_frBuffer[4] == 0 && m_frBuffer[5] == 0); // Optional: reject known-bad “all zero” sample (your existing heuristic)

   if (badHeader) LED.RED5.set();
   if (isZero)    LED.RED2.on ();

   if (badHeader || isZero) {
     data.state = DIRTY;
     return data;
   }

  const uint8_t* p = &m_frBuffer[3]; // skip status
  for (int ch=0; ch<8; ++ch) {
    int32_t val = be24_to_s32(p[0], p[1], p[2]);
    p += 3;
    data.channels[ch] = val;
  }
  return data;
}


void CA2D::onSpiDmaComplete(EventResponderRef)
{
    arm_dcache_delete( m_rxBuffer, sizeof(m_rxBuffer));
    memcpy(m_frBuffer, m_rxBuffer, sizeof(m_rxBuffer));

    digitalWriteFast(CS.A2D, HIGH);
    SPI.endTransaction();

    s_dmaActive = false;
}




