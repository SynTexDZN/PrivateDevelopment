#ifndef Buzzer_H
#define Buzzer_H

#include "main.h"
#include "Arduino.h"
#include <ESP8266WebServer.h>

class Buzzer
{
  private:
    int Pin;
  
  public:
    Buzzer(int Pin);
    
    void SETUP(String ip, String port, boolean led, ESP8266WebServer &server);

    boolean activated;
};

#endif
