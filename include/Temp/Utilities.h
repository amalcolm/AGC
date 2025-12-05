
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

void potWrite(uint8_t chipSelect, int address, int value)
{
  // gain control of the SPI port
  // and configure settings
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  // take the SS pin low to select the chip:
  digitalWrite(chipSelect, LOW);
  delayMicroseconds(5);
  //  send in the address and value via SPI:
  SPI.transfer(address);
  SPI.transfer(value);
  // take the SS pin high to de-select the chip:
  digitalWrite(chipSelect, HIGH);
  delayMicroseconds(5);
  // release control of the SPI port
 SPI.endTransaction();
}


int sensorRead(uint8_t sensorPin) {
  constexpr int LOOPS = 10;
    int totalValue = 0; 

    for (int i = 0; i < LOOPS; i++)
     totalValue = totalValue + analogRead(sensorPin);  // takes 36.2us per read    

    return totalValue / LOOPS;

}