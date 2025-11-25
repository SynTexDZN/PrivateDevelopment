#ifndef Light_H
#define Light_H

#include "main.h"
#include "Arduino.h"

class Light
{
  private:
    unsigned long previousMillis;

  public:
    Light();
    
    void SETUP(String ip, String port, int interval, boolean led);
    void UPDATE(boolean force);
    
    float light;

    boolean activated;
};

#endif
