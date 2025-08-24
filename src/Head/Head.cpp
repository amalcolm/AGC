#include "pins_arduino.h"
#include "core_pins.h"
#include "Head.h"
#include "A2D/A2D.h"
#include "Helpers.h"

CHead::CHead() : m_State(0)
{}

CHead::~CHead() { if (m_pSequence) delete[] m_pSequence; }

void CHead::setSequence( std::initializer_list<uint8_t> data ) {

  m_sequenceLength = data.size();
  m_pSequence = new uint8_t[m_sequenceLength];

  uint8_t *pW = m_pSequence;
  for ( uint8_t b : data )
    *pW++ = b;
}

CHead::StateType CHead::setNextState() {
  m_sequencePosition = (m_sequencePosition + 1) % m_sequenceLength;

  m_State = m_pSequence[m_sequencePosition];

  return m_State;
}



inline CHead::StateType CHead::getActiveState() {
  CHead::StateType state = 0x00;
  
  if (digitalReadFast(LED.RED1)) state |= LED_RED;
  if (digitalReadFast(LED.IR1 )) state |= LED_IR;
  
  return state;
}

































/*
void CHead::setPortAndMasks() {
  m_portGPIO = digitalPinToPort(PIN_GPIO_RED);
  if (digitalPinToPort(PIN_GPIO_IR) != m_portGPIO)
  {
      
  Serial.print("Pin 5 is on port: ");
  Serial.println(digitalPinToPort(5));
  
  Serial.print("Pin 6 is on port: ");
  Serial.println(digitalPinToPort(6));
  
  Serial.print("Pin 5 bit mask: 0x");
  Serial.println(digitalPinToBitMask(5), HEX);
  
  Serial.print("Pin 6 bit mask: 0x");
  Serial.println(digitalPinToBitMask(6), HEX);
  Serial.flush();
    error("Pins for IR and RED are on different ports");
  }

  m_maskGPIO_RED = digitalPinToBitMask(PIN_GPIO_RED);
  m_maskGPIO_IR  = digitalPinToBitMask(PIN_GPIO_IR);


  m_portInputRegister = portInputRegister(m_portGPIO);
}

CHead::StateType CHead::getActiveState() {
  uint32_t portValue = *m_portInputRegister;  
  return ((portValue & m_maskGPIO_RED) ? LED_RED : 0x00) | 
         ((portValue & m_maskGPIO_IR ) ? LED_IR  : 0x00);
}


uint32_t  CHead::m_portGPIO     = 0;
uint32_t  CHead::m_maskGPIO_RED = 0;
uint32_t  CHead::m_maskGPIO_IR  = 0;
volatile uint32_t* CHead::m_portInputRegister = NULL;
*/
