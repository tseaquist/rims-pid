#include "RotaryDial.h"

RotaryDial::RotaryDial(int pinA, int pinB) : encoder(pinA, pinB) {}

int RotaryDial::getTurnCount()
{
  long position = encoder.read();
  long diff = (position - previousPosition);
  int val = diff / 4;
  if(val != 0)
  {
    //Make sure previous positions aren't saved between
    //detents (multiples of 4)
    previousPosition = (position / 4) * 4;
  }
  return val;
}
