#include "CTimer.h"
#include "../teensy_compat.h"

volatile uint64_t CTimer::s_overflowCount = 0;
volatile uint64_t CTimer::s_connectTime   = 0;
volatile uint32_t CTimer::s_lastReading   = 0;

double CTimer::ticksPerSecond = F_CPU / 1.0f;
double CTimer::ticksPerMS = F_CPU / 1000.0f;
double CTimer::ticksPerUS = F_CPU / 1000000.0f;


CTimer::CTimer() {
  uint32_t reg = ARM_DWT_CTRL;
  ARM_DWT_CTRL = reg | ARM_DWT_CTRL_CYCCNTENA;  // Enable the counter
  initGPT1();
  
  
  calibration = 0;
  restart();
  calibration = elapsed();
}


bool GPT1_initialised = false;

// Teensy 4.x hardware timer base (GPT1)
extern "C" void irq_gpt1(void) {
    GPT1_SR = GPT_SR_OF1 | GPT_SR_OC1;  // clear both flags
    CTimer::time();                      // keep counter updated
}

void CTimer::initGPT1() {
  if (GPT1_initialised) return;
  
  // --- Enable clock for GPT1 in CCM ---
  uint32_t reg = CCM_CCGR1;
    reg |= CCM_CCGR1_GPT(CCM_CCGR_ON);
    CCM_CCGR1 = reg;

  // --- Reset and configure GPT1 ---
  GPT1_CR = 0;               // disable while configuring
  GPT1_PR = 0;               // no prescaler. runs at F_CPU
  GPT1_OCR1 = F_CPU / 2;     // compare value for interrupt (0.5s)
  GPT1_SR = 0x3F;            // clear all status flags
  GPT1_IR = GPT_IR_OF1IE
          | GPT_IR_OC1IE;    // enable overflow interrupt
  GPT1_CR = GPT_CR_CLKSRC(1) 
          | GPT_CR_ENMOD
          | GPT_CR_EN;       // peripheral clock, enable

   // --- Attach interrupt vector ---
  attachInterruptVector(IRQ_GPT1, irq_gpt1);
  NVIC_ENABLE_IRQ(IRQ_GPT1);
  GPT1_SR = 0x3F;            // clear any pending
  
  GPT1_initialised = true;
  
}


