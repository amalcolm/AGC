#include "CUSB.h"
#include <algorithm>



void CUSB::do_read() {
  const size_t nBytesToRead = min(Serial.available(), static_cast<size_t>(BYTE_BUFFER_SIZE));
  return;  // Disabled for now
  size_t numRead = 0;
  while (numRead < nBytesToRead) {
    auto [pBuffer, nChunk] = m_byteBuffer.getWriteChunk();
    if (nChunk == 0) break;  // buffer full

    size_t nBytes = std::min(nChunk, nBytesToRead - numRead);


    int readBytes = Serial.readBytes(reinterpret_cast<char*>(pBuffer), nBytes);
    m_byteBuffer.commitWrite(readBytes);

    if (readBytes < nBytes) break;  // issues on the serial port

    numRead += static_cast<size_t>(readBytes);
  }

}