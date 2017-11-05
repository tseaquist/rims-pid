#include "RimsUpdater.h"
#include <MemoryFree.h>

RimsUpdater::RimsUpdater()
{
    strncpy(numBuff, clearLine, 20);
    numBuff[20] = '\0';

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
  // Serial.print("freeMemory()=");
  // Serial.println(freeMemory());
  char* value = displayLines[0];
  double rimsTemp = rimsController->getRimsTemp();
  double setPoint = rimsController->setPoint;
  dtostrf(rimsTemp, 0, 1, numBuff);
  strncat(numBuff, clearLine, 20 - strlen(numBuff));
  //  01234567890123456789
  // "RIMS:###.#~###.#  AU" OR
  // "RIMS:###.#        MN"
  strncpy(value + 5, clearLine, 5);
  strncpy(value + 5, numBuff, 5);
  if(autoModeOn)
  {
    strncpy(value + 10, clearLine, 10);
    strncpy(value + 10, toWord, 1);
    dtostrf(setPoint, 0, 1, numBuff);
    strncat(numBuff, clearLine, 20 - strlen(numBuff));
    strncpy(value + 11, numBuff, 5);
    strncpy(value + 18, autoWord, 2);

  }
  else
  {
    strncpy(value + 10, clearLine, 10);
    strncpy(value + 18, manualWord, 2);
  }

  value = displayLines[1];
  double mashTemp = rimsController->getMashTemp();
  dtostrf(mashTemp, 0, 1, numBuff);
  strncat(numBuff, clearLine, 20 - strlen(numBuff));
  //  01234567890123456789
  // "MASH:###.#          ",
  strncpy(value + 5, clearLine, 5);
  strncpy(value + 5, numBuff, 5);

  // Serial.print("freeMemory()=");
  // Serial.println(freeMemory());

  value = displayLines[2];
  double amps = ammeter->getCurrent();
  double out = rimsController->output;
  dtostrf(amps, 0, 1, numBuff);
  strncat(numBuff, clearLine, 20 - strlen(numBuff));
  //  01234567890123456789
  // "AMPS:##.#   OUT:###%",
  strncpy(value + 5, clearLine, 4);
  strncpy(value + 5, numBuff, 4);
  dtostrf(out * 100.0, 0, 0, numBuff);
  strncat(numBuff, clearLine, 20 - strlen(numBuff));
  strncpy(value + 16, numBuff, 3);

  // Serial.print("freeMemory()=");
  // Serial.println(freeMemory());

  value = displayLines[3];
  //  01234567890123456789
  // "P:#### I:#### D:####"
  strncpy(value + 1, (inputMode == 1 ? setWord : valWord), 1);
  strncpy(value + 8, (inputMode == 1 ? setWord : valWord), 1);
  strncpy(value + 15, (inputMode == 1 ? setWord : valWord), 1);
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
