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
CA2D           A2D( TESTMODE ? CA2D::ModeType::TRIGGERED : CA2D::ModeType::CONTINUOUS );
CHead          Head;
CUSB           USB;

OutputPin activityLED{4};
bool Ready = false;


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

    Serial.println("Error: system halted.");
    Serial.println(hdr);
    Serial.println();

    Serial.print("Calling function: ");
    void* ra = __builtin_return_address(0);
    Serial.println((uintptr_t)ra, HEX);

    Serial.flush();

    for (;;) { digitalWrite(4, HIGH); delay(1500); digitalWrite(4, LOW); delay(500); }
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