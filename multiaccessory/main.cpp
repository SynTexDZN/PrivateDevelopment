#include "main.h"
#include "Arduino.h"
#include "FS.h"
#include <ArduinoJson.h>
#include <ESP8266httpUpdate.h>

SynTexMain::SynTexMain() {}

boolean SynTexMain::SETUP(String Version, String Services, String Buttons, String Suffix)
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

    this -> Services = Services;
    this -> Buttons = Buttons;
    this -> Suffix = Suffix;
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
    
    Serial.print("Services: ");
    Serial.println(this -> Services);
    Serial.print("Buttons: ");
    Serial.println(this -> Buttons);
    Serial.print("Suffix: ");
    Serial.println(this -> Suffix);
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

  return true;
}

void SynTexMain::LOOP()
{
  server.handleClient();
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
  
    int response = safeFetch("http://syntex.sytes.net/smarthome/check-version.php?device=" + server.arg("type"), 10, true);
  
    if(response == HTTP_CODE_OK)
    {    
      String newVersion = sender.getString();
  
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(200, "text/html", "Success");
  
      Serial.println("http://syntex.sytes.net/smarthome/ota/" + server.arg("type") + newVersion + ".bin");
  
      t_httpUpdate_return ret = ESPhttpUpdate.update("http://syntex.sytes.net/smarthome/ota/" + server.arg("type") + newVersion + ".bin");
  
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

  int response = safeFetch(BridgeIP + ":1711/serverside/init?id=" + WiFi.macAddress() + "&ip=" + WiFi.localIP().toString() + "&name=" + safeName + "&version=" + Version + "&services=" + Services + "&buttons=" + Buttons, 10, true);
  
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
  if(server.hasArg("type"))
  {
    Serial.print("Das Gerät wird zurückgesetzt ..");
  
    int response = safeFetch(BridgeIP + ":1711/serverside/remove-device?id=" + WiFi.macAddress(), 10, true);
    
    if(server.hasArg("force") || (response == HTTP_CODE_OK && sender.getString() == "Success"))
    {
      WiFiName = "";
      WiFiPass = "";
      Name = "";
    
      saveFileSystem();
      
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(200, "text/html", "Success");
  
      ESPhttpUpdate.update("http://syntex.sytes.net/smarthome/ota/" + server.arg("type") + Version + ".bin");
    
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

  if(obj["events"].as<String>() != "")
  {
    Buttons = obj["events"].as<String>();
  }

  if(obj["suffix"].as<String>() != "")
  {
    Suffix = obj["suffix"].as<String>();
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
  doc["events"] = Buttons;
  doc["suffix"] = Suffix;

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
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/html", WiFi.macAddress());

  delay(2000);

  if(server.hasArg("wifissid") && server.hasArg("wifipass"))
  {
    WiFi.begin(server.arg("wifissid"), server.arg("wifipass"));
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

      if(server.hasArg("bridge-ip"))
      {
        BridgeIP = server.arg("bridge-ip");
      }

      if(server.hasArg("device-name"))
      {
        Name = server.arg("device-name");
      }
      
      WiFiName = server.arg("wifissid");
      WiFiPass = server.arg("wifipass");

      saveFileSystem();

      ESP.restart();
    }
    else
    {
      Serial.println("Verbindung zum Netzwerk konnte nicht hergestellt werden!");

      startAccessPoint();
    }
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
