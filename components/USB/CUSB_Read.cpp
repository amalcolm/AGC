#include "CUSB.h"
#include <algorithm>



void CUSB::do_read() {
  uint32_t nAvailable = Serial.available();
  if (nAvailable == 0) return;

  if (m_handshakeComplete == false) {
    doHandshake();
    return;
  }

  uint32_t nBytesToRead = std::min(nAvailable, static_cast<uint32_t>(m_byteBuffer.available()));

  size_t numRead = 0;
  while (numRead < nBytesToRead) {
    auto [pBuffer, nChunk] = m_byteBuffer.getWriteChunk();
    if (nChunk == 0) break;  // buffer full

    size_t nBytes = min(nChunk, nBytesToRead - numRead);

    int readBytes = Serial.readBytes(reinterpret_cast<char*>(pBuffer), nBytes);
    m_byteBuffer.commitWrite(readBytes);

//    if (readBytes < nBytes) break;  // issues on the serial port

    numRead += readBytes;
  }

}