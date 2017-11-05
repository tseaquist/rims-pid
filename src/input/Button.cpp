#include "Button.h"

Button::Button(int buttonPin)
{
  this->buttonPin = buttonPin;
  pinMode(buttonPin, INPUT);
  previousButtonState = LOW;
  previousClickTime = millis();
}

bool Button::isClicked()
{
  // read the state of the pushbutton value:
  bool isClicked = false;
  int buttonState = digitalRead(buttonPin);
  unsigned long currentMillis = millis();

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState == HIGH && previousButtonState == LOW && currentMillis - previousClickTime > 250) {
    isClicked = true;
    previousClickTime = currentMillis;
  }
  previousButtonState = buttonState;
  return isClicked;
}
