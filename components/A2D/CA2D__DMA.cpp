#include "CA2D.h"
#include "Setup.h"
#include "CUSB.h"
#include "CHead.h"
#include "Helpers.h"

// buffers for DMA SPI transfers - must be 32-byte aligned for cache management on Teensy 4.x
alignas(32) uint8_t m_rxBuffer[32];
alignas(32) uint8_t m_txBuffer[32];
alignas(32) uint8_t m_frBuffer[32];


DataType CA2D::getData_DMA() {

  if (s_dmaActive) return DataType(DIRTY); 
  
  DataType data(Head.getState());  // sets timestamp and stateTime

  s_dmaActive = true;

  SPI.beginTransaction(spiSettings);
  digitalWriteFast(CS.A2D, LOW);

  arm_dcache_flush(m_txBuffer, sizeof(m_txBuffer));
  arm_dcache_delete(m_rxBuffer, sizeof(m_rxBuffer));

  Timer.addEvent(EventKind::SPI_DMA_START);

  SPI.transfer(m_txBuffer, m_rxBuffer, 32, s_spiEvent);

  setDebugData(data);

  while (s_dmaActive) yield(); // wait for DMA complete

  Timer.addEvent(EventKind::SPI_DMA_COMPLETE);
  
  dataFromFrame(m_frBuffer, data);

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

