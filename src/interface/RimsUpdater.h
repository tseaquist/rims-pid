#include "../controls/RimsController.h"
#include "../display/LCD.h"
#include "../input/Input.h"
#include "../sensor/Ammeter.h"

class RimsUpdater
{
  public:
    RimsController* rimsController;
    Ammeter* ammeter;
    Input* input;

    LCD<4, 20>* lcd;
    unsigned int pumpOutPin;
    unsigned int pumpLedPin;

    char displayLines[4][21] = {{0},{0},{0},{0}};

    bool pumpOn;
    bool rimsOn;
    bool autoModeOn;

    int inputMode;

    RimsUpdater();
    void update();
    void display();
    void pumpMode(bool onOff);
};
