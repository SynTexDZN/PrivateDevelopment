#ifndef StatelessSwitch_H
#define StatelessSwitch_H

#include "main.h"
#include "Arduino.h"
#include <ESP8266WebServer.h>

class StatelessSwitch
{
  public:
    StatelessSwitch();
    
    void SETUP(String ip, String port, String events, boolean led, ESP8266WebServer &server);
    void UPDATE(boolean force);

    boolean* button;

    boolean activated;
};

#endif
