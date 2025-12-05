
void setup() {
  pinMode(ACTIVITY_LED, OUTPUT);
  digitalWrite(ACTIVITY_LED, HIGH);


  constexpr uint32_t BAUDRATE = 57600 * 8;
  Serial.begin(BAUDRATE);
  Serial.println(" !!!! Running Code Now OK !!!!: ");

  pinMode(PIN_A1, INPUT); // analogue in
  pinMode(PIN_A0, INPUT); // analogue in
  
  pinMode(CS_A2D    , OUTPUT); 
  pinMode(CS_GAIN   , OUTPUT);
  pinMode(CS_OFFSET1, OUTPUT);
  pinMode(CS_OFFSET2, OUTPUT);

  SPI.begin();

  initA2D();
  delay(30);

  potWrite(CS_OFFSET1, 0, Offset1);
  potWrite(CS_OFFSET2, 0, Offset2);
  potWrite(CS_GAIN   , 0, Gain   );

  pinMode(27, OUTPUT);
  digitalWrite(27, LOW);

  digitalWrite(ACTIVITY_LED, LOW);  // show status - solid blue is in setup, flickering is run
}