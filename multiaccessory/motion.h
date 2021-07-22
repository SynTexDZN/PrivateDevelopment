#ifndef Motion_H
#define Motion_H

#include "main.h"
#include "Arduino.h"

class Motion
{
  private:
    int Pin;
    unsigned long previousMillis;
  
  public:
    Motion(int Pin);
    
    void SETUP(String ip, String port, int interval, boolean led);
    void UPDATE(boolean force);

    float motion;

    boolean activated;
};

#endif
