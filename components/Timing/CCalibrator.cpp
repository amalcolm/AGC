#include "CCalibrator.h"
#include "imxrt.h"



// --- SNVS_LP (Low Power) RTC Bit Definitions for Teensy 4.x (i.MX RT1062) ---
#ifndef SNVS_RTC_BITS_DEFINED
#define SNVS_RTC_BITS_DEFINED

// SNVS_LPCR bits
#define SNVS_LPCR_SRTC_ENV     (1u << 0)   // Enable Secure Real Time Counter
#define SNVS_LPCR_ALARM_EN     (1u << 2)   // Enable alarm interrupt output

// SNVS_LPSR bits
#define SNVS_LPSR_ALARM        (1u << 0)   // Alarm flag

#endif


// Static storage
static volatile bool s_ready = false;
static volatile uint32_t s_start = 0, s_end = 0;

void CCalibrator::setupRTCAlarm() {
  // Enable RTC if not already running
  SNVS_LPCR |= SNVS_LPCR_SRTC_ENV;
  while (!(SNVS_LPCR & SNVS_LPCR_SRTC_ENV)) { }  // wait

  // Clear previous alarm flag
  SNVS_LPSR |= SNVS_LPSR_ALARM;

  // Get current second count and set alarm one second later
  uint32_t now = SNVS_LPSRTCMR;
  SNVS_LPSRTCMR = now + 1;

  // Enable RTC alarm interrupt
  SNVS_LPCR |= SNVS_LPCR_ALARM_EN;

  // Attach ISR to the SNVS IRQ
  attachInterruptVector(IRQ_SNVS_IRQ, rtcISR);
  NVIC_ENABLE_IRQ(IRQ_SNVS_IRQ);
}

void CCalibrator::rtcISR() {
  SNVS_LPSR |= SNVS_LPSR_ALARM;  // clear alarm flag
  s_end = ARM_DWT_CYCCNT;
  s_ready = true;
}

double CCalibrator::measureF_CPU() {
  // Ensure DWT cycle counter enabled
  ARM_DEMCR |= ARM_DEMCR_TRCENA;
  ARM_DWT_CTRL |= ARM_DWT_CTRL_CYCCNTENA;

  // Set up alarm for +1 second
  s_ready = false;
  s_start = ARM_DWT_CYCCNT;
  setupRTCAlarm();

  // Wait for interrupt
  while (!s_ready) {
    // optional: low-power wait or yield()
  }

  NVIC_DISABLE_IRQ(IRQ_SNVS_IRQ);
  double delta = (double)((uint32_t)(s_end - s_start));
  return delta;  // cycles per 1.000000 second
}
