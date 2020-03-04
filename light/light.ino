#include "main.h"

SynTexMain m;

void setup()
{
  m.SETUP("light", "3.0.0", 10000);
}

void loop()
{
  m.LOOP();

  if(m.checkConnection())
  {
    getLight();
    //getRain();
  }
}
/*
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
*/
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
