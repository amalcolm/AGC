namespace Temp {

void updateGain()
{
  if (InZone == false) return;

     Gain_SP_value = sensorRead(   Gain_SP);

  if (   Gain_SP_value >= 924 || Gain_SP_value <= 124)  
  {
    Gain    = Gain    + 1;                // decrease gain
    if (Gain    >= 255)    Gain = 255;
    
  }
 
  if (   Gain_SP_value >= 400 && Gain_SP_value <= 624)
    {
      Gain   = Gain   - 1;              // increase gain
      if (Gain <= 0  )     Gain = 0  ;
    }



  potWrite(CS_GAIN   , 0, Gain   );
}
}