#include "Switch.h"

Switch::Switch(int switchPin)
{
  this->switchPin = switchPin;
  pinMode(switchPin, INPUT);
}

bool Switch::isUp()
{
  return digitalRead(switchPin) == HIGH;
}
