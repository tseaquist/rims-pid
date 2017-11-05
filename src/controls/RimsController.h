#ifndef RIMS_H
#define RIMS_H
#include "PID_v1.h"
#include "../sensor/Thermistor.h"
#include "SimplePwm.h"
#include <Arduino.h>

class RimsController
{
  public:
    Thermistor* tubeThermistor;
    Thermistor* mashThermistor;
    PID* pid;
    bool isOn;
    int activeLedPin;
    int outputPin;
    double kpOffset=0, kiOffset = sizeof(double), kdOffset = 2 * sizeof(double);
    double kp, ki, kd;
    double setPoint;
    double output;
    SimplePwm* pwmOutput;
    double frequencyHz = 1;

    RimsController(
      Thermistor* tubeThermistor,
      Thermistor* mashThermistor,
      int activeLedPin,
      int outputPin,
      double startSetPoint );

    void setOnState( bool isOn, bool autoMode );
    void setMode( bool autoMode );
    void setTemp( double temp );
    void incrementInput( int increment, int param );
    void setParams(double kp, double ki, double kd);
    void saveParams();
    void setManualOutput( double dutyCycle );
    double getRimsTemp();
    double getMashTemp();
    void update();
};

#endif
