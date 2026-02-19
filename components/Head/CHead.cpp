#include "pins_arduino.h"
#include "core_pins.h"
#include "CHead.h"
#include "CA2D.h"
#include "Hardware.h"
#include "Helpers.h"
#include "Setup.h"
#include "DataTypes.h"
#include "CTimer.h"
#include "Config.h"

const uint64_t CHead::MAXUINT64 = static_cast<uint64_t>(-1);
const ZTests zTest;

CHead::CHead() : m_State(UNSET), m_sequencePosition(-1) {}

CHead::~CHead() {}

void CHead::begin() {
  LED.clear();  // turn off all LEDs
}

void CHead::waitForReady() const { 

  A2D.setReadState(CA2D::ReadState::PREPARE);
 
  while (Timer.Head.waiting()) A2D.poll();
 
  A2D.setReadState(CA2D::ReadState::READ); // clear dataReady to ensure fresh read on next A2D read
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


  if (!diff && !reset) return m_State;
  m_State = newState;

  LED.write(newState);

  getHWforState().set();            // Apply hardware settings (digipots) for new state

  return m_State;
}

void CHead::clear() {
  m_State = UNSET;
  m_sequencePosition = -1;

  LED.clear();
}
