void updateOffset2()
{
  if (InZone == false) return;

  Offset2_SP_value = sensorRead(Offset2_SP);

  if (Offset2_SP_value >= 724)
  {
    Offset2 = Offset2 + 1;
    if (Offset2 >= 254) Offset2 = 254;

  }

  if (Offset2_SP_value <= 300)
  {  
    Offset2 = Offset2 - 1;
    if (Offset2 <= 1  ) Offset2 = 1  ;

  }




  potWrite(CS_OFFSET2, 0, Offset2);
}