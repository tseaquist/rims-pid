#include "RotaryDial.h"
#include "Button.h"
#include "Switch.h"

class Input
{
  public:
    RotaryDial dial;
    Button rotaryButton;
    Button pumpButton;
    Button rimsButton;
    Switch rimsModeSwitch;

  public:
    Input();

    int getTurnCount();
    bool isRotaryClick();
    bool isPumpClick();
    bool isRimsClick();
    bool isRimsModeUp();
};
