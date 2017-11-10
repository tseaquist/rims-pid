#include "SimplePwm.h"

SimplePwm::SimplePwm(double frequencyHz, int outputPin)
{
  periodMillis = (long) ( 1 / frequencyHz ) * 1000;
  this->outputPin = outputPin;
  pinMode(outputPin, OUTPUT);
  digitalWrite(outputPin, LOW);
  windowStartTime = millis();
  isOn = false;
}

void SimplePwm::setMode( bool isOn )
{
  this->isOn = isOn;
  //Turn off immediately
  if(!isOn)
  {
      digitalWrite(outputPin, LOW);
  }
}

void SimplePwm::update(double dutyCycle)
{
  unsigned long currentMillis = millis();
  unsigned long quotient = (currentMillis - windowStartTime) / periodMillis;
  windowStartTime += quotient * periodMillis;

  if ((1 - dutyCycle) * periodMillis < currentMillis - windowStartTime)
  {
    if(isOn)
    {
      digitalWrite(outputPin, HIGH);
    }
  }
  else
  {
    digitalWrite(outputPin, LOW);
  }
}
