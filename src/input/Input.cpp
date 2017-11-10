#include "Input.h"

Input::Input():
  dial(3, 2),
  rotaryButton(11),
  pumpButton(4),
  rimsButton(5),
  rimsModeSwitch(6)
{
  dial.getTurnCount();
  rotaryButton.isClicked();
  pumpButton.isClicked();
  rimsButton.isClicked();
  rimsModeSwitch.isUp();
}

int Input::getTurnCount()
{
  return dial.getTurnCount();
}

bool Input::isRotaryClick()
{
  return rotaryButton.isClicked();
}

bool Input::isPumpClick()
{
  return pumpButton.isClicked();
}

bool Input::isRimsClick()
{
  return rimsButton.isClicked();
}

bool Input::isRimsModeUp()
{
  return rimsModeSwitch.isUp();
}
