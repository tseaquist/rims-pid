#include "RimsController.h"
#include <EEPROM.h>

RimsController::RimsController(
  Thermistor* tubeThermistor,
  Thermistor* mashThermistor,
  int activeLedPin,
  int outputPin,
  double startSetPoint)
{
  this->setPoint = startSetPoint;
  this->output = 0;
  this->tubeThermistor = tubeThermistor;
  this->mashThermistor = mashThermistor;

  EEPROM.get(kpOffset, kp);
  EEPROM.get(kiOffset, ki);
  EEPROM.get(kdOffset, kd);
  this->pid = new PID(tubeThermistor->currentTemp, &output, &setPoint,
    kp, ki, kd,
    P_ON_M, DIRECT);

  this->isOn = false;

  this->pid->SetMode(MANUAL);
  this->pid->SetOutputLimits(0, 1);

  this->activeLedPin = activeLedPin;
  pinMode(activeLedPin, OUTPUT);
  digitalWrite(activeLedPin, LOW);

  this->pwmOutput = new SimplePwm(frequencyHz, outputPin);
}

void RimsController::setOnState( bool isOn, bool autoMode )
{
  if(this->isOn != isOn && !isOn && autoMode)
  {
    output = 0.0;
  }
  this->isOn = isOn;
  this->pwmOutput->setMode(isOn);
  if(isOn)
  {
    setMode(autoMode);
    digitalWrite(activeLedPin, HIGH);
  }
  else
  {
    setMode(false);
    digitalWrite(activeLedPin, LOW);
  }
}

void RimsController::setMode( bool autoMode )
{

  bool on = this->pid->GetMode() == AUTOMATIC;
  //Is state changed?
  if(autoMode != on)
  {
    if(autoMode)
    {
      this->pid->SetMode(AUTOMATIC);
    }
    else
    {
      this->pid->SetMode(MANUAL);
    }
  }
}

void RimsController::setTemp(double temp)
{
  this->setPoint = temp;
}

void RimsController::incrementInput(int increment, int param)
{
  if(param == 0)
  {
    bool autoMode = (this->pid->GetMode() == AUTOMATIC);
    if(autoMode)
    {
      setTemp(this->setPoint + increment);
    }
    else
    {
      int percent = (int)(this->output * 100.0);
      percent += increment;
      percent = percent < 0 ? 0 : percent;
      percent = percent > 100 ? 100 : percent;

      setManualOutput(percent / 100.0);
    }
  }
  else if(param == 1)
  {
    kp += increment / 10.0;
    setParams(kp, ki, kd);
  }
  else if(param == 2)
  {
    ki += increment / 10.0;
    setParams(kp, ki, kd);
  }
  else if(param == 3)
  {
    kd += increment / 10.0;
    setParams(kp, ki, kd);
  }
}

void RimsController::saveParams()
{
  EEPROM.put( kpOffset, kp );
  EEPROM.put( kiOffset, ki );
  EEPROM.put( kdOffset, kd );
}

void RimsController::setParams(double kp, double ki, double kd)
{
  this->kp = kp;
  this->ki = ki;
  this->kd = kd;
  this->pid->SetTunings(kp, ki, kd);
}

void RimsController::setManualOutput( double dutyCycle )
{
  this->output = dutyCycle;
}

double RimsController::getRimsTemp()
{
  return this->tubeThermistor->getTemp();
}

double RimsController::getMashTemp()
{
  return this->mashThermistor->getTemp();
}

void RimsController::update()
{
  this->pid->Compute();

  this->pwmOutput->update(this->output);
}
