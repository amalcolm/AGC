#include <deque>
#include "WString.h"
#include <ranges>

#include "CA2D.h" 
#include "CHead.h"
#include "CTimer.h"
#include "CMasterTimer.h"
#include "CUSB.h"
#include "Setup.h"
#include "Hardware.h"
#include "_Helpers.h"

ChipSelectPins CS;
SensorPins     SP;
ButtonPins     BUT;
LedPins        LED;
CMasterTimer   Timer;
CA2D           A2D( CA2D::ModeType::CONTINUOUS );
CHead          Head;
CUSB           USB;

OutputPin activityLED{4};
bool Ready = false;

static std::deque<PerStateHW> stateHWs;

PerStateHW& getPerStateHW(StateType state) {

  if (state == DIRTY) state = Head.getState();
  
  for (auto& hw : stateHWs)
      if (hw.state == state)
       return hw;
  
  stateHWs.emplace_back(state);
  if (Ready)
    stateHWs.back().begin();

  return stateHWs.back();
}

PerStateHW& getPerStateHW(BlockType* block) {
  return getPerStateHW(block ? block->state : DIRTY);
}

PerStateHW& getPerStateHW(DataType& data) {
  return getPerStateHW(data.state);
}



[[noreturn]] void error_impl(const char* file, int line, const char* func,
                             const char* fmt, ...)
{
    char msg[4096];
    va_list args; va_start(args, fmt);
    vsnprintf(msg, sizeof(msg), fmt, args);
    va_end(args);

    // Compose the file/line line in RAM too
    char hdr[4200];
    snprintf(hdr, sizeof(hdr), "%s:%d in %s(): %s", file, line, func, msg);

    // Try to make USB serial usable even during static init
    if (!Serial) {
        Serial.begin(115200); 
        for (int i = 0; i < 50 && !Serial; ++i) delay(10); // ~500ms max
    }

    

    auto viewForward = std::views::iota(24, 42); // pins 24 to 41 inclusive
    auto viewReverse = viewForward | std::views::reverse;
    std::vector<uint8_t> pinsForward(viewForward.begin(), viewForward.end());    pinsForward.push_back(4);
    std::vector<uint8_t> pinsReverse(viewReverse.begin(), viewReverse.end());    pinsReverse.push_back(4);

    for (auto pin : pinsForward) 
      pinMode(pin, OUTPUT);

    for (;;) {
      Serial.println("Error: system halted.");
      Serial.println(hdr);
      Serial.println();

      Serial.print("Calling function: ");
      void* ra = __builtin_return_address(0);
      Serial.println((uintptr_t)ra, HEX);

      Serial.flush();


      for (auto pin : pinsForward) {
        digitalWrite(pin, HIGH);
        delay(20);
      }
      delay(1500); 

      for (auto pin : pinsReverse) {
        digitalWrite(pin, LOW);
        delay(20);
      }
      delay(500);
    }
}

