#include "RimsUpdater.h"

RimsUpdater::RimsUpdater()
{
    Adafruit_ADS1115 ads;
    ads.setGain(GAIN_ONE);
    ads.begin();
    //ADS A0 pin
    Thermistor* rimsThermistor = new Thermistor(0, &ads, 10000,
      1.0381149079075397E-3,
      2.5144581342720644E-4,
      0.0,
      0.0);
    //ADS A1 pin
    Thermistor* mashThermistor = new Thermistor(1, &ads, 10000,
      1.0381149079075397E-3,
      2.5144581342720644E-4,
      0.0,
      0.0);
    //active led pin: 9
    //output relay pin: 10
    rimsController = new RimsController(rimsThermistor, mashThermistor, 9, 10, 150);

    //Arduino A0 pin
    ammeter = new Ammeter(0);

    input = new Input();
    lcd = new LCD<4, 20>( 0x27 );

    pumpOutPin = 7;
    pumpLedPin = 8;
    pinMode(pumpOutPin, OUTPUT);
    digitalWrite(pumpOutPin, LOW);
    pinMode(pumpLedPin, OUTPUT);
    digitalWrite(pumpLedPin, LOW);
    pumpOn = false;
    rimsOn = false;
    autoModeOn = true;
}

void RimsUpdater::pumpMode(bool onOff)
{
  if(onOff)
  {
    digitalWrite(pumpLedPin, HIGH);
    digitalWrite(pumpOutPin, HIGH);
  }
  else
  {
    digitalWrite(pumpLedPin, LOW);
    digitalWrite(pumpOutPin, LOW);
  }
}

void RimsUpdater::update()
{
  if(input->isPumpClick())
  {
    pumpOn = !pumpOn;
    pumpMode(pumpOn);
  }
  if(input->isRimsClick())
  {
    rimsOn = !rimsOn;
  }
  //Cannot have pump off with rims on
  if(rimsOn && !pumpOn)
  {
    rimsOn = false;
  }

  autoModeOn = input->isRimsModeUp();
  //Safe to call repeatedly
  rimsController->setOnState( rimsOn, autoModeOn);


  if(input->isRotaryClick())
  {
    if(autoModeOn)
    {
      inputMode = (inputMode + 1)%4;
      if(inputMode == 0)
      {
        rimsController->saveParams();
      }
    }
  }

  int turnCount = input->getTurnCount();
  if(turnCount != 0)
  {
    rimsController->incrementInput(turnCount, inputMode);
  }
  rimsController->update();
  display();
}

void RimsUpdater::display()
{
  char* value = displayLines[0];
  value[0] = '\0';
  strcpy(value, "RIMS: ");
  float rimsTemp = rimsController->getRimsTemp();
  float setPoint = rimsController->setPoint;
  dtostrf((double)rimsTemp, 0, 1, value + strlen(value));
  strcat(value, "~");
  dtostrf((double)setPoint, 0, 1, value + strlen(value));

  value = displayLines[1];
  value[0] ='\0';
  strcpy(value, "MASH: ");
  float mashTemp = rimsController->getMashTemp();
  dtostrf((double)mashTemp, 0, 1, value + strlen(value));

  value = displayLines[2];
  value[0] ='\0';
  strcpy(value, "AMPS: ");
  float amps = ammeter->getCurrent();
  int duty = (int)rimsController->output;
  dtostrf((double)amps, 0, 1, value + strlen(value));
  itoa(duty, value + strlen(value), 10);
  strcat(value, "%");

  value = displayLines[3];
  value[0] = '0';
  strcpy(value, "Kp");
  strcat(value, inputMode == 1 ? "?" : ":");
  dtostrf(rimsController->kp, 0, 1, value + strlen(value));
  strncpy(value + 7, " Ki", 7);
  strcat(value, inputMode == 2 ? "?" : ":");
  dtostrf(rimsController->ki, 0, 1, value + strlen(value));
  strncpy(value + 14, " Kd", 7);
  strcat(value, inputMode == 3 ? "?" : ":");
  dtostrf(rimsController->kd, 0, 1, value + strlen(value));

  lcd->show(displayLines);
}
