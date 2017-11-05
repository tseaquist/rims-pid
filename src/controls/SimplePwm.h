#ifndef PWM_H
#define PWM_H
#include <Arduino.h>

class SimplePwm
{
  public:
    unsigned long periodMillis;
    unsigned long windowStartTime;
    int outputPin;
    bool isOn;

    SimplePwm(double frequencyHz, int outputPin);
    void setMode(bool isOn);
    void update( double dutyCycle );
};

#endif
