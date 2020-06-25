#ifndef Light_H
#define Light_H

#include "main.h"
#include "Arduino.h"

class Light
{
  public:
    Light();
    
    void SETUP(String ip, String port, int interval, String events);
    void UPDATE(boolean force);
    
    unsigned long previousMillis;

    float light;
};

#endif
