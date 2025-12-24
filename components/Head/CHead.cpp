#include "pins_arduino.h"
#include "core_pins.h"
#include "CHead.h"
#include "CA2D.h"
#include "Setup.h"
#include "DataTypes.h"
#include "CTimer.h"

const uint64_t CHead::MAXUINT64 = static_cast<uint64_t>(-1);

CHead::CHead() : m_State(0), m_sequencePosition(-1) {
  // start with 100 microseconds settle time
  m_settleTime = static_cast<uint64_t>(100.0 / CTimer::getMicrosecondsPerTick());
}

CHead::~CHead() {}

void CHead::begin() {
  m_sequencePosition = -1;
  m_State = DIRTY;
  unsetReady();
}


void CHead::setSequence( std::vector<StateType> data ) { m_sequence = std::move(data);}

std::vector<StateType>& CHead::getSequence() {  return m_sequence;}

void CHead::setSequence(std::initializer_list<StateType> il) { m_sequence.assign(il); } // handles resizing wheres = it; does not

 double CHead::getSettleTime() const { return m_settleTime * CTimer::getMillisecondsPerTick(); }

 void CHead::setSettleTime(double milliseconds) {
    m_settleTime = static_cast<uint64_t>(milliseconds / CTimer::getMillisecondsPerTick());
  }


bool CHead::isReady() const { return Timer.time() >= m_ReadyTime; }


void CHead::waitForReady() const { 
  if (m_ReadyTime == MAXUINT64) ERROR("CHead::waitForReady called when ready time is unset");


  A2D.prepareForRead();
  while (Timer.time() < m_ReadyTime) A2D.poll();
  A2D.startRead(); // clear datReady to ensure fresh read on next A2D read
}

StateType CHead::setNextState() {
  m_ReadyTime = Timer.time() + m_settleTime;
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

  Timer.markStateChange(); // inform timer of state change for stateTime tracking

  return m_State;
}

void CHead::clear() {
  m_State = UNSET;
  m_sequencePosition = -1;

  LED.all.clear();
}

const StateType DIRTY = CHead::DIRTY;
