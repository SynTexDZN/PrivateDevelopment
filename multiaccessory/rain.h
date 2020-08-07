#ifndef Rain_H
#define Rain_H

#include "main.h"
#include "Arduino.h"

class Rain
{
  public:
    Rain();
    
    void SETUP(String ip, String port, int interval, boolean led);
    void UPDATE(boolean force);
    
    unsigned long previousMillis;

    float rain;

    boolean activated;
};

#endif
