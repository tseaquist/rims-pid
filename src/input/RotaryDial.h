#include <Encoder.h>

class RotaryDial
{
  public:
    Encoder encoder;
    long previousPosition;

  public:
    RotaryDial(int pinA, int pinB);

    int getTurnCount();
};
