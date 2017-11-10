
#include <Arduino.h>
#include "interface/RimsUpdater.h"

RimsUpdater* rims;

void setup()
{
  Serial.begin(9600L);
  rims = new RimsUpdater();
}

void loop()
{
  rims->update();
}
