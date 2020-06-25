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
  
    String Type;
    boolean Active;
    int Interval;
  
    Accessory();

    boolean SETUP(String Type, String Version, int Interval, String Events, String BridgeIP, String WebhookPort);
    int safeFetch(String URL, int Time, boolean Dots);
    
    int EventsNegative;
    boolean *EventLockNegative;
    int EventControlNegative[];

    HTTPClient sender;

    int EventsPositive;
    boolean *EventLockPositive;
    int EventControlPositive[];    
};

#endif
