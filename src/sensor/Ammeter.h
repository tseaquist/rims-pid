#include "Arduino.h"

class Ammeter
{
  public:
    unsigned int pin;
    unsigned long lastUpdate;
    double lastCurrent;
    unsigned int bitResolution = 1023;
    float refVoltage = 5;

    Ammeter(unsigned int pin);

    double getCurrent();
    void update();
};
