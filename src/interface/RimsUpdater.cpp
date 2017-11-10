#include "RimsUpdater.h"

RimsUpdater::RimsUpdater()
{
    lcd = new LCD<4, 20>( 0x27 );

    ads = new Adafruit_ADS1115();
    ads->setGain(GAIN_ONE);
    ads->begin();
    //ADS A0 pin
    Thermistor* rimsThermistor = new Thermistor(0, ads, 10000,
      1.0381149079075397E-3,
      2.5144581342720644E-4,
      0.0,
      0.0);
    //ADS A1 pin
    Thermistor* mashThermistor = new Thermistor(1, ads, 10000,
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

    pumpOutPin = 7;
    pumpLedPin = 8;
    pinMode(pumpOutPin, OUTPUT);
    digitalWrite(pumpOutPin, LOW);
    pinMode(pumpLedPin, OUTPUT);
    digitalWrite(pumpLedPin, LOW);
    pumpOn = false;
    rimsOn = false;
    autoModeOn = input->isRimsModeUp();
    inputMode = 0;
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
  if(!autoModeOn)
  {
    inputMode = 0;
  }
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
  const char* clearLine = "                    ";

  strncpy(numBuff, clearLine, 20);
  numBuff[20] = '\0';

  char* value = displayLines[0];
  double rimsTemp = rimsController->getRimsTemp();
  double setPoint = rimsController->setPoint;
  dtostrf(rimsTemp, 0, 1, numBuff);
  strncat(numBuff, clearLine, 20 - strlen(numBuff));
  strncpy(value + 5, clearLine, 5);
  strncpy(value + 5, numBuff, 5);
  if(autoModeOn)
  {
    strncpy(value + 10, clearLine, 10);
    strncpy(value + 11, "SET:", 4);
    dtostrf(setPoint, 0, 1, numBuff);
    strncat(numBuff, clearLine, 20 - strlen(numBuff));
    strncpy(value + 15, numBuff, 5);
  }
  else
  {
     strncpy(value + 10, clearLine, 10);
  }

  value = displayLines[1];
  double mashTemp = rimsController->getMashTemp();
  dtostrf(mashTemp, 0, 1, numBuff);
  strncat(numBuff, clearLine, 20 - strlen(numBuff));
  strncpy(value + 5, clearLine, 5);
  strncpy(value + 5, numBuff, 5);

  strncpy(value + 10, clearLine, 10);
  strncpy(value + 10, clearLine, 10);
  strncpy(value + 11, autoModeOn ? "AUT:" : "MNL:", 4);
  strncpy(value + 15, rimsOn ? "ON " : "OFF", 3);

  value = displayLines[2];
  double amps = abs(ammeter->getCurrent());
  amps = amps < 0 ? -amps : amps;
  double out = rimsController->output;
  dtostrf(amps, 0, 1, numBuff);
  strncat(numBuff, clearLine, 20 - strlen(numBuff));
  strncpy(value + 5, clearLine, 4);
  strncpy(value + 5, numBuff, 4);
  dtostrf(out * 100.0, 0, 0, numBuff);
  strncat(numBuff, "%", 20 - strlen(numBuff));
  strncat(numBuff, clearLine, 20 - strlen(numBuff));
  strncpy(value + 15, numBuff, 5);

  const char* setWord = "?";
  const char* valWord = ":";
  value = displayLines[3];
  strncpy(value + 1, (inputMode == 1 ? setWord : valWord), 1);
  strncpy(value + 8, (inputMode == 2 ? setWord : valWord), 1);
  strncpy(value + 15, (inputMode == 3 ? setWord : valWord), 1);
  dtostrf(rimsController->kp, 0, 1, numBuff);
  strncat(numBuff, clearLine, 20 - strlen(numBuff));
  strncpy(value + 2, clearLine, 4);
  strncpy(value + 2, numBuff, 4);
  dtostrf(rimsController->ki, 0, 1, numBuff);
  strncat(numBuff, clearLine, 20 - strlen(numBuff));
  strncpy(value + 9, clearLine, 4);
  strncpy(value + 9, numBuff, 4);
  dtostrf(rimsController->kd, 0, 1, numBuff);
  strncat(numBuff, clearLine, 20 - strlen(numBuff));
  strncpy(value + 16, clearLine, 4);
  strncpy(value + 16, numBuff, 4);

  const char* passValDisplay[4] =
  {
    displayLines[0],
    displayLines[1],
    displayLines[2],
    displayLines[3]
  };

  lcd->show(passValDisplay);
}
