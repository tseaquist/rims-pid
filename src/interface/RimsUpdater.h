#include "../controls/RimsController.h"
#include "../display/LCD.h"
#include "../input/Input.h"
#include "../sensor/Ammeter.h"
#include <math.h>

class RimsUpdater
{
  public:
    RimsController* rimsController;
    Ammeter* ammeter;
    Input* input;
    Adafruit_ADS1115* ads;

    unsigned long lastUpdate;

    LCD<4, 20>* lcd;
    unsigned int pumpOutPin;
    unsigned int pumpLedPin;

    char displayLines[4][21] =
    {
     // 0   1   2   3   4   5   6   7   8   9   0   1   2   3   4   5   6   7   8   9
      {'R','I','M','S',':','#','#','#','.','#',' ','S','E','T',':','#','#','#','.','#'},
      {'M','A','S','H',':','#','#','#','.','#',' ','M','N','L',':','O','F','F',' ',' '},
      {'A','M','P','S',':','#','#','.','#',' ',' ','O','U','T',':','#','#','#','%',' '},
      {'P',':','#','#','#','#',' ','I',':','#','#','#','#',' ','D',':','#','#','#','#'}
    };
    char numBuff[21];

    bool rimsOn;
    bool autoModeOn;

    int inputMode;

    RimsUpdater();
    void update();
    void display(bool force);
    void pumpMode(bool onOff);
};
