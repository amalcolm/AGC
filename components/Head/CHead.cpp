#include "pins_arduino.h"
#include "core_pins.h"
#include "CHead.h"
#include "CA2D.h"
#include "Setup.h"

CHead::CHead() : m_State(0) {}

CHead::~CHead() { if (m_pSequence) delete[] m_pSequence; }

void CHead::init() {
  LED.all.deactivate();
  m_sequencePosition = -1;
  m_State = DIRTY;
  
  //setPortAndMasks();
}

void CHead::setSequence( std::initializer_list<uint32_t> data ) {

  m_sequenceLength = data.size();
  m_pSequence = new uint32_t[m_sequenceLength];

  uint32_t *pW = m_pSequence;
  for ( uint32_t b : data )
    *pW++ = b;
}

std::vector<CHead::StateType> CHead::getSequence() {
    std::vector<CHead::StateType> seq;
    for (int i = 0; i < m_sequenceLength; i++)
      seq.push_back(m_pSequence[i]);
    return seq;
}


CHead::StateType CHead::setNextState() {
  m_sequencePosition = (m_sequencePosition + 1) % m_sequenceLength;

  const StateType oldState = m_State;
  const StateType newState = m_pSequence[m_sequencePosition];

  StateType diff = (newState ^ oldState) & VALIDBITS;
  
  if (!diff) return m_State;
  m_State = newState;

  while (diff) {
      const int  i  = __builtin_ctz(diff);          // index of lowest set bit
      const bool on = (newState >> i) & 1u;

      if (i < NUM_LEDS) 
          digitalWriteFast(LED.RED1 + i               , on ? HIGH : LOW);
      else // i in [16..24]
          digitalWriteFast(LED.IR1 + (i - IR_STARTBIT), on ? HIGH : LOW);

      diff &= diff - 1;                             // clear LOWEST bit using magic
  }

  return m_State;
}



inline CHead::StateType CHead::getActiveState() {  // NOT COMPLETE
  CHead::StateType state = 0x00;
  
  if (digitalReadFast(LED.RED1)) state |= RED1;
  if (digitalReadFast(LED.IR1 )) state |= IR1;
  
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
