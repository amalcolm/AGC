namespace Temp {

// *******************************************************************
// Function To Read the led 
// *******************************************************************

void configWrite(std::initializer_list<uint8_t> data);


void initA2D()
{
  
  SPI.beginTransaction(SPISETTINGS);

  configWrite({ 0x11 }); // SDATAC
  
  // CHnSe5et; 8 channels     1    2     3     4     5     6     7     8  
  configWrite({ 0x45, 0x07, 0x00, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61} );    // 00 = electrode, 01 =short, 05 = test sig

  // config2 register
  configWrite({ 0x42, 0x00, 0xD4 });

  // config3 register
  configWrite({ 0x43, 0x00, 0xE0 });

  SPI.endTransaction();

}


// *******************************************************************
// Function To Read the led 
// *******************************************************************
long readTheLEDData(){
  SPI.beginTransaction(SPISETTINGS);

  digitalWrite(CS_A2D, LOW);  // RDATA
  delayMicroseconds(5);
  SPI.transfer(0x12);

  byte dataGrab[30];
  long data[8];

  for(int i = 0; i < 30 ; i++){
    dataGrab[i] = SPI.transfer(0x00);
  }

  digitalWrite(CS_A2D, HIGH);
  delayMicroseconds(10);

  for(int q = 1 ; q < 7 ; q++){
    int bitLength = 24;
    long rawValue = ((long)dataGrab[3*q] << 16) | ((long)dataGrab[3*q+1] << 8) | dataGrab[3*q+2];

    if (rawValue & (1 << (bitLength - 1))) {
      // If the MSB is 1, then it is a negative number.
      rawValue = rawValue - (1L << bitLength);
    }

    data[q] = rawValue;
  }
  
  SPI.endTransaction();
  return data[1];
}

void configWrite(std::initializer_list<uint8_t> data) {
  digitalWrite(CS_A2D, LOW);
  delayMicroseconds(5);
  for (uint8_t byte : data) {
    SPI.transfer(byte);
  }
  delayMicroseconds(5);
  digitalWrite(CS_A2D, HIGH);
  delayMicroseconds(10);
}

}
