#include "pins_arduino.h"
void Output(int A2D_value)
{
  A2D_value = analogRead(Offset1_SP);

  Serial.print("Min:0\tMax:1024\t"); 

  Serial.print("Sens:");    Serial.print(A2D_value);  Serial.print("\t");
  Serial.print("Off1:");    Serial.print(Offset1);    Serial.print("\t");
//Serial.print("Off2:");    Serial.print(Offset2);    Serial.print("\t");
//Serial.print("Gain:");    Serial.print(Gain   );    Serial.print("\t");


  Serial.println();
}
