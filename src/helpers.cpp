#include "core_pins.h"
#include <Arduino.h>
#include "helpers.h"
#include "Setup.h"
#include "A2D/A2D.h"

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

void error(const char *msg)
{
  Serial.println("Error: system halted.");
  Serial.println(msg);
  Serial.println("--End");
  Serial.flush();
  while (true)
  {
    LED.activity.write(HIGH);
    delay(1500);
    LED.activity.write(LOW );
    delay(500);
  }
}


ChipSelectPins CS;
ProbePointPins PP;
ButtonPins     BUT;
LedPins        LED;
Timer          timer;
CA2D           A2D(CA2D::ModeType::CONTINUOUS);

Hardware        HW(&CS, &PP, &BUT, &LED, &timer, &A2D);


