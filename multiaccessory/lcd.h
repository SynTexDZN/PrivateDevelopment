#ifndef LCD_H
#define LCD_H

#include "main.h"
#include "Arduino.h"

class LCD
{
  private:    
    unsigned long timeMillis;
    unsigned long previousMillis;

    int Interval;
    
  public:
    LCD();
    
    void SETUP(String IP, String Port, int Interval, boolean Backlight, String Name, String Version);
    void UPDATE(int i, String* Infos);

    boolean activated;
};

#endif
