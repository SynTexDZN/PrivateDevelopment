#ifndef Climate_H
#define Climate_H

#include "main.h"
#include "Arduino.h"

class Climate
{
  public:
    Climate();
    
    void SETUP(String ip, String port, int interval, boolean led);
    void UPDATE(boolean force);

    unsigned long previousMillis;

    float temp;
    float hum;

    boolean activated;
};

#endif
