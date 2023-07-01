#include "main.h"
#include "Arduino.h"
#include "FS.h"
#include <ArduinoJson.h>
#include <ESP8266httpUpdate.h>

SynTexMain::SynTexMain() {}

boolean SynTexMain::SETUP(String Version, String Services, String Suffix, String Buttons, int Interval)
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

  server.on("/wifi", [this]{ scanWiFi(); });
  server.on("/settings", HTTP_POST, [this]{ saveWiFiSettings(); });
  server.on("/", [this]{ server.sendHeader("Access-Control-Allow-Origin", "*"); server.send(200, "text/plain", ""); });
  server.on("/reset", [this]{ resetDevice(); });
  server.on("/restart", [this]{ server.sendHeader("Access-Control-Allow-Origin", "*"); server.send(200, "text/plain", "Success"); delay(2000); ESP.restart(); });
  server.on("/version", [this]{ server.sendHeader("Access-Control-Allow-Origin", "*"); server.send(200, "text/plain", this -> Version); });
  server.on("/update", [this]{ updateDevice(); });
  server.on("/refresh", [this]{ loadDatabaseSettings(); });
  server.on("/config", [this]{ serviceOverride(); });
  server.begin();
  
  if(!loadFileSystem())
  {
    Serial.println("Eine leere Config wurde geladen!");

    this -> Services = Services;
    this -> Suffix = Suffix;
    this -> Buttons = Buttons;
    this -> Interval = Interval;
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

    Serial.println("-------------");
    
    if(this -> Services != "null")
    {
    Serial.print("Services: ");
    Serial.println(this -> Services);
    }
    
    if(this -> Suffix != "null")
    {
      Serial.print("Suffix: ");
      Serial.println(this -> Suffix);
    }

    if(this -> Buttons != 0)
    {
    Serial.print("Buttons: ");
    Serial.println(this -> Buttons);
    }

    if(this -> Interval != 0)
    {
      Serial.print("Interval: ");
      Serial.println(this -> Interval);
    }
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

    Name = "SynTex-" + rnd;

    saveFileSystem();
  }

  this -> Version = Version;

  return true;
}

void SynTexMain::LOOP()
{
  server.handleClient();
}

void SynTexMain::setupWiFi()
{
  if(WiFiName != "")
  {
    WiFi.begin(WiFiName, WiFiPass);
    Serial.print("Verbindung wird hergestellt ..");

    int i = 0;
    
    while(WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
      ++i;

      if(i == 120 * 30)
      {
         digitalWrite(LED_BUILTIN, HIGH);
      }

      if(i % 120 == 0)
      {
        delay(300000);

        Serial.println();
        Serial.print("Verbindung wird hergestellt ..");
      }
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
}

void SynTexMain::serviceOverride()
{
  if(server.hasArg("services"))
  {
    Services = server.arg("services");

    saveFileSystem();

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "Success");

    delay(2000);

    ESP.restart();
  }

  if(server.hasArg("view-services"))
  {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", Services);
  }

  if(server.hasArg("buttons"))
  {
    Buttons = server.arg("buttons");

    saveFileSystem();

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "Success");

    delay(2000);

    ESP.restart();
  }

  if(server.hasArg("view-buttons"))
  {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", Buttons);
  }

  if(server.hasArg("suffix"))
  {
    Suffix = server.arg("suffix");

    saveFileSystem();

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "Success");

    delay(2000);

    ESP.restart();
  }

  if(server.hasArg("view-suffix"))
  {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", Suffix);
  }
}

void SynTexMain::updateDevice()
{
  /*
  ESPhttpUpdate.onStart(update_started);
  ESPhttpUpdate.onEnd(update_finished);
  ESPhttpUpdate.onProgress(update_progress);
  ESPhttpUpdate.onError(update_error);
  */

  if(server.hasArg("type"))
  {
    Serial.print("Nach Updates suchen ..");

    String* request = safeFetch("http://syntex-cloud.com:8888/check-version?type=" + server.arg("type"), 10, true);
  
    if(request[0] == "OK")
    {    
      String newVersion = request[1];
  
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(200, "text/html", "Success");
  
      Serial.println("http://syntex-cloud.com:8800/bin/" + server.arg("type") + newVersion + ".bin");

      t_httpUpdate_return ret = ESPhttpUpdate.update(client, "http://syntex-cloud.com:8800/bin/" + server.arg("type") + newVersion + ".bin");
  
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
}

boolean SynTexMain::loadDatabaseSettings()
{
  String safeName = Name;

  safeName.replace(" ", "%");

  Serial.print("Mit der SynTex Bridge verbinden ..");

  String* request = safeFetch(BridgeIP + ":1711/serverside/init?id=" + WiFi.macAddress() + "&ip=" + WiFi.localIP().toString() + "&name=" + safeName + "&version=" + Version + "&services=" + Services + "&buttons=" + Buttons, 10, true);
  
  if(request[0] == "OK")
  {    
    Serial.println("SynTex Bridge erfolgreich verbunden!");

    StaticJsonDocument<400> doc;
    deserializeJson(doc, request[1]);
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
}

void SynTexMain::resetDevice()
{
  if(server.hasArg("type"))
  {
    Serial.print("Das Gerät wird zurückgesetzt ..");
  
    String* request = safeFetch(BridgeIP + ":" + WebhookPort + "/devices?id=" + WiFi.macAddress() + "&remove=CONFIRM", 10, true);
    
    if(server.hasArg("force") || (request[0] == "OK" && request[1] == "Success"))
    {
      WiFiName = "";
      WiFiPass = "";
      Name = "";
    
      saveFileSystem();
      
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(200, "text/html", "Success");

      ESPhttpUpdate.update(client, "http://syntex-cloud.com:8800/bin/" + server.arg("type") + Version + ".bin");
    
      delay(2000);
      
      ESP.reset();
    }
    else
    {
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(200, "text/html", "Error");
    }
  }
}

boolean SynTexMain::loadFileSystem()
{
  File configFile = SPIFFS.open("/config.json", "r");
  
  if(!configFile)
  {
    return false;
  }

  size_t size = configFile.size();
  
  if(size > 1024)
  {
    Serial.println("Config Datei ist zu groß!");
    return false;
  }

  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);

  StaticJsonDocument<400> doc;
  DeserializationError error = deserializeJson(doc, buf.get());
  
  if(error)
  {
    Serial.println("JSON konnte nicht gelesen werden!");
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

  if(obj["services"].as<String>() != "")
  {
    Services = obj["services"].as<String>();
  }

  if(obj["suffix"].as<String>() != "")
  {
    Suffix = obj["suffix"].as<String>();
  }

  if(obj["events"].as<String>() != "")
  {
    Buttons = obj["events"].as<String>();
  }

  if(obj["interval"].as<String>() != "")
  {
    Interval = obj["interval"].as<int>();
  }

  return true;
}

boolean SynTexMain::saveFileSystem()
{
  StaticJsonDocument<400> doc;
  doc["ssid"] = WiFiName;
  doc["pass"] = WiFiPass;
  doc["name"] = Name;
  doc["bridge"] = BridgeIP;
  doc["services"] = Services;
  doc["suffix"] = Suffix;
  doc["events"] = Buttons;
  doc["interval"] = Interval;

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
  if(server.hasArg("plain"))
  {
    StaticJsonDocument<400> doc;
    deserializeJson(doc, server.arg("plain"));
    JsonObject obj = doc.as<JsonObject>();
    
    if(obj["wifissid"].as<String>() != "null" && obj["wifipass"].as<String>() != "null")
    {
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(200, "text/html", WiFi.macAddress());
    
      delay(2000);
    
      WiFi.begin(obj["wifissid"].as<String>(), obj["wifipass"].as<String>());
      Serial.print("Verbindung wird getestet ..");
  
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
        BridgeIP = "http://syntex.local";
  
        if(obj["bridge-ip"].as<String>() != "null")
        {
          BridgeIP = obj["bridge-ip"].as<String>();
        }
  
        if(obj["device-name"].as<String>() != "null")
        {
          Name = obj["device-name"].as<String>();
        }
        
        WiFiName = obj["wifissid"].as<String>();
        WiFiPass = obj["wifipass"].as<String>();
  
        saveFileSystem();
  
        ESP.restart();
      }
      else
      {
        Serial.println("Verbindung zum Netzwerk konnte nicht hergestellt werden!");
  
        startAccessPoint();
      }
    }
    else
    {
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(200, "text/html", "Error");
    }
  }
  else
  {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/html", "Error");
  }
}

boolean SynTexMain::checkConnection()
{
  if(WiFi.status() == WL_CONNECTED && WiFiName != "" && WiFiName == WiFi.SSID())
  {
    return true;
  }
  
  return false;
}

String netze[64][3];
int found = 0;

void SynTexMain::scanWiFi()
{
  if(server.hasArg("view-current"))
  {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "{" + String('"') + "ssid" + String('"') + ":" + String('"') + WiFiName + String('"') + "," + String('"') + "rssi" + String('"') + ":" + String(WiFi.RSSI()) + "}");
  }
  else
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
  
        netze[i][0] = WiFi.SSID(i);
        netze[i][1] = String(WiFi.RSSI(i));
        netze[i][2] = String(WiFi.encryptionType(i));
        
        Serial.print(" (");
        Serial.print(WiFi.RSSI(i));
        Serial.print(")");
        Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      }
  
      String html = "{";
  
      if(found != 0)
      {
        html += String('"') + "connected" + String('"') + ":";
  
        if(WiFi.status() == WL_CONNECTED)
        {
          html += "{" + String('"') + "ssid" + String('"') + ":" + String('"') + WiFiName + String('"') + "," + String('"') + "rssi" + String('"') + ":" + WiFi.RSSI() + "}";
        }
        else
        {
          html += "null";
        }
  
        html += "," + String('"') + "available" + String('"') + ":[";
  
        for(int i = 0; i < found; ++i)
        {
          html += "{" + String('"') + "ssid" + String('"') + ":" + String('"') + netze[i][0] + String('"') + "," + String('"') + "rssi" + String('"') + ":" + netze[i][1] + "," + String('"') + "pass" + String('"') + ":" + String('"') + netze[i][2] +String('"') + "}";
    
          if(i < found - 1)
          {
            html += ",";
          }
        }
    
        html += "]";
      }

      html += "}";
  
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(200, "text/plain", html);
    }
  }
}

String* SynTexMain::safeFetch(String URL, int Time, boolean Dots)
{
  HTTPClient sender;
  
  int response;
  int counter = 0;
  
  do
  {
    sender.begin(client, URL);
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

  String* request = new String[2];
  request[0] = (response == HTTP_CODE_OK) ? "OK" : "ERROR";
  request[1] = sender.getString();

  sender.end();

  return request;
}
