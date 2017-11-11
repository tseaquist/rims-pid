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
    lastUpdate = millis();
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
  bool forceDisplay = false;
  if(input->isPumpClick())
  {
    forceDisplay = true;
    pumpOn = !pumpOn;
    pumpMode(pumpOn);
  }
  if(input->isRimsClick())
  {
    forceDisplay = true;
    rimsOn = !rimsOn;
  }
  //Cannot have pump off with rims on
  if(rimsOn && !pumpOn)
  {
    rimsOn = false;
  }

  if(autoModeOn != input->isRimsModeUp())
  {
    forceDisplay = true;
  }
  autoModeOn = input->isRimsModeUp();
  if(!autoModeOn)
  {
    inputMode = 0;
  }
  //Safe to call repeatedly
  rimsController->setState( rimsOn, autoModeOn);


  if(input->isRotaryClick())
  {
    forceDisplay = true;
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
    forceDisplay = true;
    rimsController->incrementInput(turnCount, inputMode);
  }
  rimsController->update();
  display(forceDisplay);
}

void RimsUpdater::display(bool force)
{
  unsigned long now = millis();
  unsigned long timeChange = (now - lastUpdate);
  if(timeChange < 500 && !force)
  {
    return;
  }

  lastUpdate = now;
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
    dtostrf(setPoint, 0, 0, numBuff);
    strncat(numBuff, clearLine, 20 - strlen(numBuff));
    strncpy(value + 15, numBuff, 3);
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
  strncpy(value + 11, autoModeOn ? "ATM:" : "MNL:", 4);
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


  value = displayLines[3];

  strncpy(value, clearLine, 20);
  if( inputMode == 0 && autoModeOn )
  {
    strncpy(value + 0, "P:", 2);
    strncpy(value + 7, "I:", 2);
    strncpy(value + 14, "D:", 2);
    //Display 0.1% per deg
    dtostrf(rimsController->kp * 1000, 0, 0, numBuff);
    strncat(numBuff, clearLine, 20 - strlen(numBuff));
    strncpy(value + 2, clearLine, 4);
    strncpy(value + 2, numBuff, 4);
    //Display 0.1% per (deg*sec)
    dtostrf(rimsController->ki * 1000, 0, 0, numBuff);
    strncat(numBuff, clearLine, 20 - strlen(numBuff));
    strncpy(value + 9, clearLine, 4);
    strncpy(value + 9, numBuff, 4);
    //Display 0.1% per (deg/sec)
    dtostrf(rimsController->kd * 1000, 0, 0, numBuff);
    strncat(numBuff, clearLine, 20 - strlen(numBuff));
    strncpy(value + 16, clearLine, 4);
    strncpy(value + 16, numBuff, 4);
  }
  else if(autoModeOn)
  {
    if(inputMode == 1)
    {
      strncpy(value + 0, "P:", 2);
      //Display 0.1% per deg
      dtostrf(rimsController->kp * 1000, 0, 0, numBuff);
      strncat(numBuff, clearLine, 20 - strlen(numBuff));
      strncpy(value + 2, numBuff, 4);
      strncpy(value + 7, "0.1%/Dg", 13);
    }
    else if(inputMode == 2)
    {
      strncpy(value + 0, "I:", 2);
      //Display 0.1% per (deg*sec)
      dtostrf(rimsController->ki * 1000, 0, 0, numBuff);
      strncat(numBuff, clearLine, 20 - strlen(numBuff));
      strncpy(value + 2, numBuff, 4);
      strncpy(value + 7, "0.1%/(Dg*Sec)", 13);
    }
    else if(inputMode == 3)
    {
      strncpy(value + 0, "D:", 2);
      //Display 0.1% per (deg/sec)
      dtostrf(rimsController->kd * 1000, 0, 0, numBuff);
      strncat(numBuff, clearLine, 20 - strlen(numBuff));
      strncpy(value + 2, numBuff, 4);
      strncpy(value + 7, "0.1%/(Dg/Sec)", 13);
    }
  }

  const char* passValDisplay[4] =
  {
    displayLines[0],
    displayLines[1],
    displayLines[2],
    displayLines[3]
  };

  lcd->show(passValDisplay);
}
