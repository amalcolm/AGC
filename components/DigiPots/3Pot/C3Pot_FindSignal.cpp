#include "C3Pot.h"
#include "DataTypes.h"
#include "Hardware.h"
#include "CUSB.h"

void C3Pot::findSignal()
{
  top.setLevel(DIGIPOT_MAX_FOR_PHOTODIODE);
  bot.setLevel(CAutoPot::POT_MIN);
  mid.setLevel(CAutoPot::POT_MIDPOINT);


  bool signalFound = false;

  int initialHILO = 0; // -1 for low, +1 for high, 0 for unknown
  int HILO = 0;
  int dataNum = 0;

  while (top.getLevel() - bot.getLevel() > GAP_NORMAL && dataNum < CFG::MAX_BLOCKSIZE) {

    readSensor();
    
    HILO = (lastSensorValue() < CAutoPot::SENSOR_MIDPOINT) ? -1 : +1;  if (initialHILO == 0) initialHILO = HILO;

    switch (signalFound)
    {
      case false:
        if (HILO == initialHILO)
          mid.offsetLevel( -HILO );
        else
          signalFound = true;
        break;


      case true:  // called once signal is found

        switch (HILO) {
          case -1: top.offsetLevel(-1); break;
          case +1: bot.offsetLevel(+1); break;
        }

        if (mid.getLevel() != CAutoPot::POT_MIDPOINT) {

          int delta = mid.getLevel() - CAutoPot::POT_MIDPOINT;
          int sign = (delta > 0) - (delta < 0);
          delta = sign * std::clamp(abs(delta) * 1/4, 1, 3);

          mid.offsetLevel( -delta  );  // drag mid to centre
        }
        break;
    }

    delayMicroseconds(100); // signalFound ? 500 : 50 );
    dataNum++;
  }

  initialHILO = 0;
  double lastDelta = 0, delta = 0;
  
  while (true) {
    lastDelta = delta;
    readSensor();
    
    delta = abs(lastSensorValue() - CAutoPot::SENSOR_MIDPOINT);
    HILO = (lastSensorValue() < CAutoPot::SENSOR_MIDPOINT) ? -1 : +1;  if (initialHILO == 0) initialHILO = HILO;

    if (HILO != initialHILO)
      break;

    mid.offsetLevel( -HILO );
    delayMicroseconds(100);
  }

  if (delta < lastDelta)  // if we just crossed over the optimal point, step back
  { 
    mid.offsetLevel( +HILO );
    delayMicroseconds(100);
    readSensor();
  }

  printDebug(false);


  for (int i = 0; i < 2; i++) {
    readSensor();
    delayMicroseconds(100);
    printDebug(true);
  }
  
}