#ifndef StatusLED_H
#define StatusLED_H

#include "main.h"
#include "Arduino.h"

class StatusLED
{
  private:    
    unsigned long fadeMicros;
    unsigned long flashMillis;
    unsigned long blinkMillis;

    boolean LED;
    
    int Fade;
    int Flash;
    int Blink;

  public:
    StatusLED();
    
    void SETUP(boolean LED, int Fade);
    void UPDATE();
    void setupRGB();
    void finishSetupRGB();
    void setRGB(int red, int green, int blue);
    void flashRGB(int red, int green, int blue, int t);
    void blinkRGB(int red, int green, int blue, int s);

    boolean activated;
};

#endif
