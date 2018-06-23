#include "Arduino.h"

class Ammeter
{
  public:
    unsigned int pin;
    unsigned long lastUpdate;
    double current = 0;
    double squareSum = 0;
    int numReads = 0;
    unsigned int bitResolution = 1023;
    double refVoltage = 5;

    Ammeter(unsigned int pin);

    double getCurrent();
    void update();
};
