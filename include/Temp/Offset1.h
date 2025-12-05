void updateOffset1()
{


  Offset1_SP_value = sensorRead(Offset1_SP);

  if (Offset1_SP_value >= 652)  //984
  {
    Offset1 = Offset1 + 1;
    if (Offset1 >= 254) Offset1 = 254;
    InZone = false;
  }

  if (Offset1_SP_value <= 362)  //984
  {
    Offset1 = Offset1 - 1;
    if (Offset1 <= 1  ) Offset1 = 1  ;
    InZone = false;
  }

  if (Offset1_SP_value < 924 && Offset1_SP_value > 100)
    InZone = true;

  potWrite(CS_OFFSET1, 0, Offset1);
}