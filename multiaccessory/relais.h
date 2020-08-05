#ifndef Relais_H
#define Relais_H

#include "main.h"
#include "Arduino.h"
#include <ESP8266WebServer.h>

class Relais
{
  public:
    Relais();
    
    void SETUP(String ip, String port, String events, boolean led, ESP8266WebServer &server);

    boolean activated;
};

#endif
