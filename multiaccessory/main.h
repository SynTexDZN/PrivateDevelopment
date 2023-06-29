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
    int Interval;
    boolean LED;
  
    SynTexMain();

    boolean SETUP(String Version, String Services, String Buttons, String Suffix);
    void LOOP();
    void setupWiFi();
    boolean checkConnection();
    String* safeFetch(String URL, int Time, boolean Dots);

    String Services;
    String Buttons;
    String Suffix;
    
    ESP8266WebServer server;
};

#endif
