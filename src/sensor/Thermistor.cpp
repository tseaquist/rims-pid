#include "Thermistor.h"

Thermistor::Thermistor(unsigned int pin, Adafruit_ADS1115* ads, double resistor, double A1, double A2, double A3, double A4)
{
  this->pin = pin;
  this->ads = ads;
  this->resistor = resistor;
  this->steinhartA1 = A1;
  this->steinhartA2 = A2;
  this->steinhartA3 = A3;
  this->steinhartA4 = A4;

  lastUpdate = millis();
  update();
}

double Thermistor::getResistance()
{
  if(ads->getGain() != GAIN_ONE)
  {
    Serial.print(F("BAD GAIN: "));
    Serial.println(ads->getGain());
    ads->setGain(GAIN_ONE);
    Serial.print(F("CORRECTED GAIN: "));
    Serial.println(ads->getGain());
  }

  int16_t adc = ads->readADC_SingleEnded(pin);
  double v = ((double)adc) * voltsPerBit;
  double resistance = voltage * resistor / v - resistor;
  return resistance;
}

double Thermistor::getTemp()
{
  unsigned long now = millis();
  if(now - lastUpdate > 1000)
  {
    lastUpdate = now;
    update();
  }
  return *currentTemp;
}

void Thermistor::update()
{
  double resistance = getResistance();
  double lnResitance = log(resistance);
  //Build up the number in the stages.
  double temp = steinhartA4;
  temp = temp * lnResitance + steinhartA3;
  temp = temp * lnResitance + steinhartA2;
  temp = temp * lnResitance + steinhartA1;
  //Final part is to invert
  temp = 1.0 / temp;
  *currentTemp = (9.0 * (temp - 273.0) / 5.0 + 32.0);
}
