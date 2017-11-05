#include <Arduino.h>

class Button
{
  protected:
    int buttonPin;
    int previousButtonState;
    unsigned long previousClickTime;

  public:
    Button(int buttonPin);
    bool isClicked();
};
