#ifndef Relais_H
#define Relais_H

#include "main.h"
#include "Arduino.h"
#include <ESP8266WebServer.h>

class Relais
{
  private:
    int Pin;
  
  public:
    Relais(int Pin);
    
    void SETUP(String ip, String port, boolean led, ESP8266WebServer &server);

    boolean activated;
};

#endif
