#include "Ammeter.h"

Ammeter::Ammeter(unsigned int pin)
{
  this->pin = pin;
  //Take the pin out of pull-up mode.
  //Arduino analog pins are 14-19 when accessed in digital mode.
  pinMode(14 + pin, INPUT);
  digitalWrite(14 + pin, LOW);
  //Throw away first read.
  analogRead(pin);

  lastUpdate = millis();
  update();
}

double Ammeter::getCurrent()
{
  unsigned long now = millis();
  lastUpdate = now;
  update();
  return lastCurrent;
}

void Ammeter::update()
{
  int bits = analogRead(pin);
  double volts = bits * refVoltage / bitResolution;
  //100mV per Amp
  double amps = (volts - refVoltage / 2.0) * 10.0;
  lastCurrent = amps;
}
