#ifndef Climate_H
#define Climate_H

#include "main.h"
#include "Arduino.h"

class Climate
{
  private:
    int Pin;
    float humPrevious;
    unsigned long previousMillis;
  
  public:
    Climate(int Pin);
    
    void SETUP(String ip, String port, int interval, boolean led);
    void UPDATE(boolean force);

    void setDHT22();

    float temp;
    float tempExact;

    float hum;
    float humExact;

    boolean activated;
};

#endif
