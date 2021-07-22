#ifndef Temperature_H
#define Temperature_H

#include "main.h"
#include "Arduino.h"

class Temperature
{
  private:
    int Pin;
    unsigned long previousMillis;
  
  public:
    Temperature(int Pin);
    
    void SETUP(String ip, String port, int interval, boolean led);
    void UPDATE(boolean force);

    float temp;
    float tempExact;

    boolean activated;
};

#endif
