#ifndef Contact_H
#define Contact_H

#include "main.h"
#include "Arduino.h"

class Contact
{
  private:
    int Pin;
  
  public:
    Contact(int Pin);
    
    void SETUP(String ip, String port, int interval, boolean led);
    void UPDATE(boolean force);
    
    unsigned long previousMillis;

    float contact;

    boolean activated;
};

#endif
