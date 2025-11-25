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
    void serviceOverride();
    
    WiFiClient client;
    
  public:
    String WebhookPort;
    String BridgeIP;
    
    String WiFiName;
    String WiFiPass;
    String Version;
    String Name;
  
    boolean Active;
    boolean LED;
    int Interval;
  
    SynTexMain();

    boolean SETUP(String Version, String Services, String Suffix, String Buttons, int Inverval);
    void LOOP();
    void setupWiFi();
    boolean checkConnection();
    String* safeFetch(String URL, int Time, boolean Dots);

    String Services;
    String Suffix;
    String Buttons;
    
    ESP8266WebServer server;
};

#endif
