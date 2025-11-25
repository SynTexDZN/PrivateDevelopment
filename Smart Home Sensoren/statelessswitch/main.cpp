#include "main.h"
#include "Arduino.h"
#include "FS.h"
#include <ArduinoJson.h>
#include <ESP8266httpUpdate.h>

SynTexMain::SynTexMain()
{
  Serial.println("New object created!");
}

String SetupEvents;

boolean SynTexMain::SETUP(String Type, String Version, int Interval, String Events)
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("-------------");
  Serial.println("ESP Gestartet");
  Serial.print("Version: ");
  Serial.println(Version);
  Serial.println("-------------");

  pinMode(LED_BUILTIN, OUTPUT);

  if(!SPIFFS.begin())
  {
    Serial.println("Es konnte nicht auf das Dateisystem zugegriffen werden");
    //return;
  }
  
  if(!loadFileSystem())
  {
    Serial.println("Eine leere Config wurde geladen!");
  }
  else
  {
    Serial.println("Config geladen!");
    
    Serial.print("WiFi Name: ");
    Serial.println(WiFiName);
    Serial.print("WiFi Pass: ");
    Serial.println(WiFiPass);
    Serial.print("Bridge IP: ");
    Serial.println(BridgeIP);
  }

  Serial.println("-------------");

  if(Name == "")
  {
    String rnd;
    char *letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    
    for(int i = 0; i < 5; i++)
    {
      byte randomValue = random(0, 36);
      rnd += letters[randomValue];
    }

    Name = char(toupper(Type.charAt(0))) + Type.substring(1) + "-" + rnd;

    saveFileSystem();
  }

  this -> Version = Version;
  this -> Type = Type;
  this -> Interval = Interval;
  SetupEvents = Events;
  
  if(WiFiName != "")
  {
    WiFi.begin(WiFiName, WiFiPass);
    Serial.print("Verbindung wird hergestellt ..");

    int i = 0;
    
    while(WiFi.status() != WL_CONNECTED && i < 120)
    {
      delay(500);
      Serial.print(".");
      ++i;
    }
    
    Serial.println();

    if(WiFi.status() == WL_CONNECTED)
    {
      Serial.print("Verbunden! IP-Adresse: ");
      Serial.println(WiFi.localIP());
      
      WiFi.softAPdisconnect(true);

      loadDatabaseSettings();

      digitalWrite(LED_BUILTIN, HIGH);
    }
    else
    {
      Serial.println("Verbindung zum Netzwerk konnte nicht hergestellt werden!");
      startAccessPoint();
    }
  }
  else
  {
    startAccessPoint();
  }

  server.on("/wifi", [this]{ scanWiFi(); });
  server.on("/settings", HTTP_POST, [this]{ saveWiFiSettings(); });
  server.on("/", [this]{ server.sendHeader("Access-Control-Allow-Origin", "*"); server.send(200, "text/plain", ""); });
  server.on("/reset", [this]{ resetDevice(); });
  server.on("/restart", [this]{ server.sendHeader("Access-Control-Allow-Origin", "*"); server.send(200, "text/plain", ""); delay(2000); ESP.restart(); });
  server.on("/version", [this]{ server.sendHeader("Access-Control-Allow-Origin", "*"); server.send(200, "text/plain", this -> Version); });
  server.on("/update", [this]{ updateDevice(); });
  server.on("/refresh", [this]{ loadDatabaseSettings(); });
  server.begin();

  return true;
}

void SynTexMain::LOOP()
{
  server.handleClient();
}

void SynTexMain::updateDevice()
{
  /*
  ESPhttpUpdate.onStart(update_started);
  ESPhttpUpdate.onEnd(update_finished);
  ESPhttpUpdate.onProgress(update_progress);
  ESPhttpUpdate.onError(update_error);
  */

  Serial.print("Nach Updates suchen ..");

  int response = safeFetch("http://syntex-cloud.com/smarthome/check-version.php?device=" + Type, 10, true);

  if(response == HTTP_CODE_OK)
  {    
    String newVersion = sender.getString();

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/html", "Success");

    Serial.println("http://syntex-cloud.com/smarthome/ota/" + Type + newVersion + ".bin");

    t_httpUpdate_return ret = ESPhttpUpdate.update("http://syntex-cloud.com/smarthome/ota/" + Type + newVersion + ".bin");

    switch(ret)
    {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;
  
      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;
  
      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
    }
  }
  else
  {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/html", "Error");
  }
}

boolean SynTexMain::loadDatabaseSettings()
{
  String safeName = Name;

  safeName.replace(" ", "%");

  Serial.print("Mit der SynTex Bridge verbinden ..");

  int response = safeFetch(BridgeIP + "/init?mac=" + WiFi.macAddress() + "&ip=" + WiFi.localIP().toString() + "&type=" + Type + "&name=" + safeName + "&version=" + Version + "&refresh=" + Interval + "&buttons=" + SetupEvents, 10, true);
  
  if(response == HTTP_CODE_OK)
  {    
    Serial.println("SynTex Bridge erfolgreich verbunden!");

    StaticJsonDocument<400> doc;
    deserializeJson(doc, sender.getString());
    JsonObject obj = doc.as<JsonObject>();

    Name = obj["name"].as<String>();
    Active = obj["active"].as<int>();
    Interval = obj["interval"].as<int>();
    LED = obj["led"].as<int>();
    WebhookPort = obj["port"].as<String>();

    Serial.println("-------------");

    Serial.print("WebhookPort: ");
    Serial.println(WebhookPort);

    Serial.println("-------------");

    Serial.print("Name: ");
    Serial.println(Name);
    Serial.print("Aktiv: ");
    Serial.println(Active ? "An" : "Aus");
    Serial.print("Interval: ");
    Serial.println(Interval);
    Serial.print("LED: ");
    Serial.println(LED ? "An" : "Aus");
    Serial.print("Events: ");

    EventsNegative = 0; 
    EventsPositive = 0; 

    JsonArray events = obj["events"].as<JsonArray>();

    for(JsonVariant v : events)
    {
      if(v.as<int>() < 0)
      {
        EventControlNegative[EventsNegative] = -v.as<int>();
        Serial.print("< " + String(EventControlNegative[EventsNegative]) + "  ");
        EventsNegative++;
      }
      else
      {
        EventControlPositive[EventsPositive] = v.as<int>();
        Serial.print("> " + String(EventControlPositive[EventsPositive]) + "  ");
        EventsPositive++;
      }
    }

    EventLockNegative = new boolean [EventsNegative];
    
    for(int i = 0; i < EventsNegative; i++)
    {
      EventLockNegative[i] = false;
    }

    EventLockPositive = new boolean [EventsPositive];
    
    for(int i = 0; i < EventsPositive; i++)
    {
      EventLockPositive[i] = false;
    }

    if(EventsPositive + EventsNegative == 0)
    {
      Serial.print("/");
    }
    
    Serial.println("");
    Serial.println("-------------");

    saveFileSystem();

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/html", "Success");
    
    return true;
  }
  else
  {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/html", "Error");
    
    return false;
  }
  
  sender.end();
}

void SynTexMain::resetDevice()
{
  Serial.print("Das Gerät wird zurückgesetzt ..");

  int response = safeFetch(BridgeIP + "/remove-device?mac=" + WiFi.macAddress() + "&type=" + Type, 10, true);
  
  if(response == HTTP_CODE_OK && sender.getString() == "Success")
  {
    WiFiName = "";
    WiFiPass = "";
    Name = "";
  
    saveFileSystem();
    
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/html", "Success");

    ESPhttpUpdate.update("http://syntex-cloud.com/smarthome/ota/" + Type + Version + ".bin");
  
    delay(2000);
    
    ESP.reset();
  }
  else
  {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/html", "Error");
  }
  
  sender.end();
}

boolean SynTexMain::loadFileSystem()
{
  File configFile = SPIFFS.open("/config.json", "r");
  
  if(!configFile)
  {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  
  if(size > 1024)
  {
    Serial.println("Config file size is too large");
    return false;
  }

  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, buf.get());
  
  if(error)
  {
    Serial.println("Failed to parse config file");
    return false;
  }

  JsonObject obj = doc.as<JsonObject>();

  if(obj["ssid"].as<String>() != "")
  {
    WiFiName = obj["ssid"].as<String>();
  }

  if(obj["pass"].as<String>() != "")
  {
    WiFiPass = obj["pass"].as<String>();
  }

  if(obj["name"].as<String>() != "")
  {
    Name = obj["name"].as<String>();
  }

  if(obj["bridge"].as<String>() != "")
  {
    BridgeIP = obj["bridge"].as<String>();
  }

  return true;
}

boolean SynTexMain::saveFileSystem()
{
  StaticJsonDocument<200> doc;
  doc["ssid"] = WiFiName;
  doc["pass"] = WiFiPass;
  doc["name"] = Name;
  doc["bridge"] = BridgeIP;

  File configFile = SPIFFS.open("/config.json", "w");
  
  if(!configFile)
  {
    Serial.println("Die Config konnte nicht gespeichert werden!");
    return false;
  }

  serializeJson(doc, configFile);
  return true;
}

void SynTexMain::startAccessPoint()
{
  Serial.print("Name: ");
  Serial.println(Name);
  
  Serial.println("Access Point wird gestartet ..");
  
  WiFi.softAP(Name);
  
  Serial.print("Access Point IP Adresse: ");
  Serial.println(WiFi.softAPIP());
}

void SynTexMain::saveWiFiSettings()
{
  BridgeIP = "http://syntex.local";

  if(server.hasArg("bridge-ip"))
  {
    BridgeIP = server.arg("bridge-ip");
  }

  if(server.hasArg("device-name"))
  {
    Name = server.arg("device-name");
  }
  
  if(server.hasArg("wifissid") && server.hasArg("wifipass"))
  {   
    WiFiName = server.arg("wifissid");
    WiFiPass = server.arg("wifipass");
    saveFileSystem();
  }

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/html", WiFi.macAddress());

  setup();
}

boolean SynTexMain::checkConnection()
{
  if(WiFi.status() == WL_CONNECTED && WiFiName != "" && WiFiName == WiFi.SSID())
  {
    return true;
  }
  
  return false;
}

String netze[64];
int found = 0;

void SynTexMain::scanWiFi()
{
  Serial.println("WLAN Netzwerke suchen ..");

  found = WiFi.scanNetworks();
  
  if(found == 0)
  {
    Serial.println("Keine Netzwerke gefunden!");
  }
  else
  {
    Serial.print(found);
    Serial.println(" Netzwerke gefunden!");
    
    for(int i = 0; i < found; ++i)
    {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));

      netze[i] = WiFi.SSID(i);
      
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
    }

    String html = "";

    if(found != 0)
    {
      html += "[";

      for(int i = 0; i < found; ++i)
      {
        html += '"' + netze[i] + '"';
  
        if(i < found - 1)
        {
          html += ",";
        }
      }
  
      html += "]";
    }

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", html);
  }
}

int SynTexMain::safeFetch(String URL, int Time, boolean Dots)
{
  int response;
  int counter = 0;
  
  do
  {
    sender.begin(URL);
    response = sender.GET();

    if(response != HTTP_CODE_OK)
    {
      delay(500);
      counter++;

      if(Dots)
      {
        Serial.print(".");
      }
    }
  }
  while(response != HTTP_CODE_OK && counter < Time * 2);

  if(Dots)
  {
    Serial.println();
  }

  return response;
}
