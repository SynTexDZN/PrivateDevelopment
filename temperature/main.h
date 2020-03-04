#ifndef SynTexMain_H
#define SynTexMain_H

#include "Arduino.h"
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

class SynTexMain
{
  public:
    SynTexMain();
    void SETUP(String Type, String Version, int Interval);
    void LOOP();
    boolean loadFileSystem();
    boolean saveFileSystem();
    void startAccessPoint();
    void saveWiFiSettings();
    boolean checkConnection();
    void resetDevice();
    void scanWiFi();
    void updateDevice();
    boolean loadDatabaseSettings();
    ESP8266WebServer server;
    HTTPClient sender;
    int Interval;
    String Type;

  private:
    String WiFiName;
    String WiFiPass;
    String Version;
    String Name;
    boolean LED;
    boolean SceneControl;
};

#endif
