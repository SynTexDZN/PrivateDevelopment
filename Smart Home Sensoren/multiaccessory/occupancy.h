#ifndef Occupancy_H
#define Occupancy_H

#include "main.h"
#include "Arduino.h"

class Occupancy
{
  private:
    int Pin;
    unsigned long previousMillis;
  
  public:
    Occupancy(int Pin);
    
    void SETUP(String ip, String port, int interval, boolean led);
    void UPDATE(boolean force);

    float occupancy;

    boolean activated;
};

#endif
