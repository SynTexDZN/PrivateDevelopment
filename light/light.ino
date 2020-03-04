#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266WebServer.h>

HTTPClient sender;
ESP8266WebServer server(80);

String WiFiName;
String WiFiPass;
int SensorID = 8;
String SensorName = "";
String SensorType = "light";
String Version = "1.0.0";

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("ESP Gestartet");
  Serial.print("Version: ");
  Serial.println(Version);

  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }
  
  if (!loadConfig()) {
    Serial.println("Failed to load config");
  } else {
    Serial.println("Config geladen!");
    Serial.print("WiFi Name: ");
    Serial.println(WiFiName);
    Serial.print("WiFi Pass: ");
    Serial.println(WiFiPass);
    Serial.print("Name: ");
    Serial.println(SensorName);
    Serial.print("ID: ");
    Serial.println(SensorID);
  }

  if(SensorName == "")
  {
    String rnd;
    char *letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    
    for(int i = 0; i < 5; i++)
    {
      byte randomValue = random(0, 37);
      rnd += letters[randomValue];
    }
    
    SensorName = (char)toupper(SensorType.charAt(0)) + SensorType.substring(1, SensorType.length()) + "-" + rnd;

    saveConfig();
  }
  
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
      server.stop();
      WiFi.softAPdisconnect(true);
      if(SensorID <= 0)
      {
        setupDevice();
      }
    }
    else
    {
      Serial.println("Verbindung zum Netzwerk konnte nicht hergestellt werden!");
      startAP();
    }
  }
  else
  {
    startAP();
  }
}

void loop()
{
  checkUpdates();
  server.handleClient();
  
  if(WiFi.status() == WL_CONNECTED && WiFiName != "")
  {
    getLight();
    //getRain();
  }
}

unsigned long previousUpdateMillis = -120000;
long updateInterval = 120000;

void checkUpdates()
{
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousUpdateMillis >= updateInterval)
  {
    previousUpdateMillis = currentMillis;   

    Serial.println("Prüfe auf Updates ..");
    
    sender.begin("http://syntex.local/ota/check-version.php?device=" + SensorType);
    int response = sender.GET();
    
    if(response == HTTP_CODE_OK)
    {    
      if(sender.getString() != Version)
      {
        Serial.println("Update gefunden! Installation ..");
        
        WiFiClient client;
        t_httpUpdate_return ret = ESPhttpUpdate.update(client, "http://syntex.local/ota/" + SensorType + sender.getString() + ".bin");
      
        switch (ret) {
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
        Serial.println("Das Gerät hat bereits die neuste Firmware!");
      }
    }
    else
    {
      Serial.println("Update Server ist nicht erreichbar!");
    }
  
    sender.end();
  }
}

bool loadConfig() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, buf.get());
  if (error) {
    Serial.println("Failed to parse config file");
    return false;
  }

  JsonObject obj = doc.as<JsonObject>();

  WiFiName = obj["ssid"].as<String>();
  WiFiPass = obj["pass"].as<String>();  
  SensorName = obj["name"].as<String>();

  if(SensorID == 0)
  {
    SensorID = obj["id"].as<int>();
  }

  return true;
}

bool saveConfig() {
  StaticJsonDocument<200> doc;
  doc["ssid"] = WiFiName;
  doc["pass"] = WiFiPass;
  doc["id"] = SensorID;
  doc["name"] = SensorName;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  serializeJson(doc, configFile);
  return true;
}

void startAP()
{
  Serial.println("Access Point wird gestartet ..");
  WiFi.softAP(SensorName);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("Access Point IP Adresse: ");
  Serial.println(myIP);

  server.on("/", handleRoot);
  server.on("/setup", setupDevice);
  server.on("/data", HTTP_POST, saveData);
  server.begin();
  Serial.println("Web Konfigurator gestartet!");
}

void setupDevice()
{
  sender.begin("http://syntex.local/add-sensor.php?type=temperature&name=" + SensorName);
  int response = sender.GET();
  
  if(response == HTTP_CODE_OK)
  {    
    SensorID = sender.getString().toInt();
    Serial.println(SensorID);
    saveConfig();
  }

  sender.end();

  sender.begin("http://syntex.local/add-sensor.php?type=humidity&name=" + SensorName);
  sender.GET();
  sender.end();
}

void saveData()
{
  if(server.hasArg("ssid") && server.hasArg("pass"))
  {   
    WiFiName = server.arg("ssid");
    WiFiPass = server.arg("pass");
    saveConfig();
  }

  String html = "<h1>Einstellungen wurden aktualisiert!</h1>";
  html += "<p>Neustart wird eingeleitet ..</p>";
  
  server.send(200, "text/html", html);

  setup();
}

String netze[64];
int found = 0;

void scanWiFi()
{
  Serial.println("WLAN Netzwerke suchen ..");

  found = WiFi.scanNetworks();
  
  if (found == 0)
  {
    Serial.println("Keine Netzwerke gefunden!");
  }
  else
  {
    Serial.print(found);
    Serial.println(" Netzwerke gefunden!");
    for (int i = 0; i < found; ++i)
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
  }
}

void handleRoot() {
  scanWiFi();

  String html = "<h1>Suche dein Heimnetzwerk raus</h1>";

  html += "<form action=\"/data\" method=\"POST\">";
  
  for (int i = 0; i < found; ++i)
  {
    html += "<input type=\"radio\" name=\"ssid\" id=\"" + netze[i] + "\" value=\"" + netze[i] + "\"><label for=\"" + netze[i] + "\"> " + netze[i] + "</label></br>";
  }

  html += "</br><input type=\"password\" name=\"pass\" placeholder=\"Netzwerk Passwort\" required></br><input type=\"submit\" value=\"Verbinden\"></form>";
  
  server.send(200, "text/html", html);
}

float light;
unsigned long previousMillisL = -30000;
long intervalL = 30000;

void getLight()
{
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillisL >= intervalL)
  {
    previousMillisL = currentMillis;   
 
    float lighttmp = 1024 - analogRead(A0);
    
    if(lighttmp != light)
    {
      light = lighttmp;
      sender.begin("http://192.168.188.121:51828/?accessoryId=sensor" + ((String)SensorID) + "&value=" + String(light));
      sender.GET();
      sender.end();

      /*
      if(light > 150 && light < 150)
      {
        sender.begin("http://192.168.188.121:51828/?accessoryId=statelessswitch1&buttonName=led-an&event=0");
        sender.GET();
        sender.end();

        Serial.println("( LEDs ) Scene wird aktiviert!");
      }

      if(light == 0)
      {
        sender.begin("http://192.168.188.121:51828/?accessoryId=statelessswitch1&buttonName=rolladen-runter&event=0");
        sender.GET();
        sender.end();

        Serial.println("( Rolladen ) Scene wird aktiviert!");
      }
      */
    }
    
    Serial.println("Licht: " + String(light) + " lux");
  }
}

String rain;
unsigned long previousMillisR = -30000;
long intervalR = 30000;

void getRain()
{
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillisR >= intervalR)
  {
    previousMillisR = currentMillis;   

    String raintmp = "false";

    if(digitalRead(D1) == 0)
    {
      raintmp = "true";
    }
    
    if(raintmp != rain)
    {
      rain = raintmp;
      sender.begin("http://192.168.188.121:51828/?accessoryId=sensor" + ((String)SensorID) + "&state=" + rain);
      sender.GET();
      sender.end();
    }
    
    Serial.println("Regen: " + rain);
  }
}
