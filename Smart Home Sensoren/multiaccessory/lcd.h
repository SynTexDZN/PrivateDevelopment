#ifndef LCD_H
#define LCD_H

#include "main.h"
#include "Arduino.h"
#include <ESP8266WebServer.h>

class LCD
{
  private:    
    unsigned long timeMillis;
    unsigned long previousMillis;

    int Interval;

    String text;
    
  public:
    LCD();
    
    void INIT();
    void SETUP(String IP, String Port, int Interval, boolean Backlight, String Name, String Version, ESP8266WebServer &server);
    void UPDATE(int i, String* Infos);

    void setText(String text);

    boolean activated;
};

#endif
