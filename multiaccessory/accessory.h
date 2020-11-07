#ifndef Accessory_H
#define Accessory_H

#include "Arduino.h"
#include <ESP8266HTTPClient.h>

class Accessory
{
  public:
    String WebhookPort;
    String BridgeIP;
    
    String Version;
    String Name;
  
    boolean Active;
    int Interval;
    boolean LED;
  
    Accessory();

    boolean SETUP(String Version, int Interval, String Buttons, String BridgeIP, String WebhookPort, boolean LED);
    int safeFetch(String URL, int Time, boolean Dots);
    
    HTTPClient sender;

    int ButtonCount;
    boolean *ButtonLockPositive;
    int *ButtonControlPositive;    
};

#endif
