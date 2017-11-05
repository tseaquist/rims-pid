#ifndef TEMP_H
#define TEMP_H

#include "Arduino.h"
#include <Adafruit_ADS1015.h>
#include <math.h>

class Thermistor
{
  public:
    unsigned int pin;
    Adafruit_ADS1115* ads;
    double resistor;
    double steinhartA1; //First Steinhart-Hart coefficient.
    double steinhartA2; //Second Steinhart-Hart coefficient.
    double steinhartA3; //Third Steinhart-Hart coefficient.
    double steinhartA4; //Fourth Steinhart-Hart coefficient.

    unsigned long lastUpdate;
    double* currentTemp;
    //Corresponds to GAIN 1
    double voltsPerBit = 0.000125;
    double voltage = 3.3;

  public:
    Thermistor(unsigned int pin, Adafruit_ADS1115* ads, double resistor, double A1, double A2, double A3, double A4);
    void update();
    float getTemp();
    double getResistance();
};
#endif
