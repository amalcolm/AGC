#include <map>
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
CA2D           A2D(CA2D::ModeType::TRIGGERED);
CHead          Head;
CUSB           USB;


struct PerStateHW& getPerStateHW(CA2D::BlockType* block) {
    static std::map<CHead::StateType, PerStateHW> stateMap;

    const auto state = Head.getState();
    auto [it, inserted] = stateMap.try_emplace(state, state);
    if (inserted) it->second.begin();
    return it->second;
}



void error(const char *msg, ...)
{
  constexpr unsigned int PRINTF_BUFFER_SIZE = 1024;
  
  char buffer[PRINTF_BUFFER_SIZE];
  va_list args;
  va_start(args, msg);
  vsnprintf(buffer, sizeof(buffer)-1, msg, args);
  va_end(args);

  Serial.println("Error: system halted.");
  Serial.println(buffer);
  Serial.println("--End");
  Serial.flush();
  while (true)
  {
    LED.all.set();
    delay(1500);
    LED.all.clear();
    delay(500);
  }
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