#ifndef Climate_H
#define Climate_H

#include "main.h"
#include "Arduino.h"

class Climate
{
  private:
    int Pin;
  
  public:
    Climate(int Pin);
    
    void SETUP(String ip, String port, int interval, boolean led);
    void UPDATE(boolean force);

    unsigned long previousMillis;

    float temp;
    float hum;

    float tempExact;
    float humExact;

    float humPrevious;

    boolean activated;
};

#endif
