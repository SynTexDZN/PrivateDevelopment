#ifndef SynTexMain_H
#define SynTexMain_H

#include "Arduino.h"
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

class SynTexMain
{
  private:
    boolean loadFileSystem();
    boolean saveFileSystem();
    void startAccessPoint();
    void saveWiFiSettings();
    void resetDevice();
    void scanWiFi();
    void updateDevice();
    boolean loadDatabaseSettings();
    
  public:
    String WiFiName;
    String WiFiPass;
    String Version;
    String Name;
  
    String Type;
    int Interval;
    boolean LED;
    int WebhookPort;
    String BridgeIP;
  
    SynTexMain();

    boolean SETUP(String Type, String Version, int Interval);
    void LOOP();
    boolean checkConnection();
    
    int EventsNegative;
    boolean *EventLockNegative;
    int EventControlNegative[];

    ESP8266WebServer server;
    HTTPClient sender;

    int EventsPositive;
    boolean *EventLockPositive;
    int EventControlPositive[];    
};

#endif
