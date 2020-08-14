#ifndef Motion_H
#define Motion_H

#include "main.h"
#include "Arduino.h"

class Motion
{
  private:
    int Pin;
  
  public:
    Motion(int Pin);
    
    void SETUP(String ip, String port, int interval, boolean led);
    void UPDATE(boolean force);
    
    unsigned long previousMillis;

    float motion;

    boolean activated;
};

#endif
