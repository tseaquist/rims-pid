#include <Arduino.h>

class Switch
{
  protected:
    int switchPin;

  public:
    Switch(int switchPin);
    bool isUp();
};
