#include "pins_arduino.h"
#include "core_pins.h"
#include "CHead.h"
#include "CA2D.h"
#include "Setup.h"
#include "DataTypes.h"
#include "CTimer.h"
#include "Config.h"

const uint64_t CHead::MAXUINT64 = static_cast<uint64_t>(-1);

CHead::CHead() : m_State(0), m_sequencePosition(-1) { }

CHead::~CHead() {}

void CHead::begin() {
  m_sequencePosition = -1;
  m_State = DIRTY;
 }

std::vector<StateType>& CHead::getSequence() {  return m_sequence;}

void CHead::setSequence( std::vector<StateType> data ) { 
  if (data.size() > 0) m_sequence = std::move(data); else ERROR("CHead::setSequence: empty sequence"); 
}


void CHead::setSequence(std::initializer_list<StateType> il) {
  if (il  .size() > 0) m_sequence.assign(il);        else ERROR("CHead::setSequence: empty sequence"); 
}

void CHead::waitForReady() const { 

  A2D.prepareForRead();
 
  while (Timer.Head.waiting()) A2D.poll();
 
  A2D.startRead(); // clear dataReady to ensure fresh read on next A2D read
 }

StateType CHead::setNextState() {
  Timer.markStateChange(); // inform timer of state change for stateTime tracking

  const bool reset = (m_sequencePosition == -1) || Pins::flashReset;
  if (reset) Pins::flashReset = false; // only use FlashReset once, and set it at start
  
  const StateType oldState = reset ? UNSET : m_State;

  A2D.setBlockState(oldState);  // set the current block (the one which has been filled) to the old state
                                // this also swaps the block and sets the filled block to be sent to USB

  m_sequencePosition = (m_sequencePosition + 1) % m_sequence.size();

  const StateType newState = m_sequence[m_sequencePosition];

  StateType diff = (newState ^ oldState) & VALIDBITS;


  if (!diff) return m_State;
  m_State = newState;


  // Update only the changed LEDs using bit manipulation
  while (diff) {
      const int  i  = __builtin_ctz(diff);          // index of lowest set bit
      
      const uint8_t led = Pins::pinForBit(i);          // corresponding pin number  
      const bool on = ((newState >> i) & 1u) ^ LED.Inverted; // desired state

      digitalWriteFast(led, on ? HIGH : LOW);
 
      diff &= diff - 1;                             // clear LOWEST bit using magic
  }


  return m_State;
}

void CHead::clear() {
  m_State = UNSET;
  m_sequencePosition = -1;

  LED.all.clear();
}

const StateType DIRTY = CHead::DIRTY;
