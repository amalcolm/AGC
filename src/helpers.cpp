#include <deque>
#include "WString.h"

#include "CA2D.h" 
#include "CHead.h"
#include "CTimer.h"
#include "CUSB.h"
#include "Setup.h"
#include "Hardware.h"
#include "Helpers.h"

ChipSelectPins CS;
ProbePointPins PP;
ButtonPins     BUT;
LedPins        LED;
CTimer         Timer;
CA2D           A2D(CA2D::ModeType::CONTINUOUS);
CHead          Head;
CUSB           USB;

OutputPin activityLED{4};



PerStateHW& getPerStateHW(StateType state) {
  static std::deque<PerStateHW> stateHWs;

  if (state == DIRTY) state = Head.getState();
  for (auto& hw : stateHWs) {
      if (hw.state == state) return hw;
  }
  PerStateHW newHW(state);
  stateHWs.push_back(newHW);
  return stateHWs.back();
}

PerStateHW& getPerStateHW(BlockType* block) {
  return getPerStateHW(block ? block->state : DIRTY);
}

PerStateHW& getPerStateHW(DataType& data){
  return getPerStateHW(data.state);
}

PerStateHW& getPerStateHW(){
  return getPerStateHW(DIRTY);
}

__attribute__((noinline)) static bool print_frame(unsigned n)
{
    void* ra = nullptr;
    switch (n) {
        case 0: ra = __builtin_return_address(0); break;
        case 1: ra = __builtin_return_address(1); break;
        case 2: ra = __builtin_return_address(2); break;
        case 3: ra = __builtin_return_address(3); break;
        case 4: ra = __builtin_return_address(4); break;
        case 5: ra = __builtin_return_address(5); break;
        default: return false;
    }
    if (!ra) return false;
    Serial.printf("#%u %p\n", n, ra);
    return true;
}

[[noreturn]] void error_impl(const char* file, int line, const char* func,
                             const char* fmt, ...)
{
    char msg[1024];
    va_list args; va_start(args, fmt);
    vsnprintf(msg, sizeof(msg), fmt, args);
    va_end(args);

    Serial.println("Error: system halted.");
    Serial.printf("%s:%d in %s(): %s\n", file, line, func, msg);
    Serial.println("Backtrace (newest first):");

    for (unsigned n = 0; n < 6; ++n) {
        if (!print_frame(n)) break;   // stop when a frame isn’t available
    }

    Serial.println("--End");
    Serial.flush();
    for (;;) { LED.all.set(); delay(1500); LED.all.clear(); delay(500); }
}



const char* getSketch() {
  static String result; // Static to ensure it lives beyond function return
 
  String path = __FILE__;
  int lastSlash = path.lastIndexOf('\\');
  
  if (lastSlash > 0) {
    int previousSlash = path.lastIndexOf('\\', lastSlash - 1);
    if (previousSlash >= 0) {
      result = path.substring(previousSlash + 1, lastSlash);
      return result.c_str();
    }
  }
  
  if (lastSlash >= 0) {
    path = path.substring(lastSlash + 1);
  }
  
  int dot = path.lastIndexOf('.');  if (dot >= 0) path = path.substring(0, dot);
  
  result = path;
  return result.c_str();
}