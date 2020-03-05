#include "main.h"
#include <Wire.h>
#include <BH1750.h>

SynTexMain m;
BH1750 lightMeter;

void setup()
{
  m.SETUP("light", "3.0.0", 10000);

  Wire.begin();
  lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE_2);
}

void loop()
{
  m.LOOP();

  if(m.checkConnection())
  {
    getLight();
    getRain();
  }
}
/*

void checkUpdates()
{
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
*/
float light;
boolean sunsetScene;
boolean nightScene;
unsigned long previousMillis = -10000000;

void getLight()
{
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis >= m.Interval)
  {
    previousMillis = currentMillis;   
 
    float lighttmp = lightMeter.readLightLevel();
    
    if(lighttmp != light)
    {
      light = lighttmp;
      m.sender.begin("http://syntex.local:1710/devices?mac=" + WiFi.macAddress() + "&type=" + m.Type + "&value=" + String(light));
      m.sender.GET();
      m.sender.end();

      if(light < 100 && !sunsetScene && m.SceneControl)
      {
        m.sender.begin("http://syntex.local:1710/devices?mac=" + WiFi.macAddress() + "&event=0");
        m.sender.GET();
        m.sender.end();

        sunsetScene = true;

        Serial.println("( LEDs ) Scene wird aktiviert!");
      }

      if(light <= 1 && !nightScene && m.SceneControl)
      {
        m.sender.begin("http://syntex.local:1710/devices?mac=" + WiFi.macAddress() + "&event=1");
        m.sender.GET();
        m.sender.end();

        nightScene = true;

        Serial.println("( Rolladen ) Scene wird aktiviert!");
      }

      if(light > 400 && (sunsetScene || nightScene) && m.SceneControl)
      {
        sunsetScene = false;
        nightScene = false;

        Serial.println("Scenen wurden zurückgesetzt!");
      }
    }
    
    Serial.println("Licht: " + String(light) + " lux");
  }
}

boolean rain;

void getRain()
{
  boolean raintmp = digitalRead(2);
    
  if(raintmp != rain)
  {
    rain = raintmp;

    if(rain)
    {
      m.sender.begin("http://syntex.local:1710/devices?mac=" + WiFi.macAddress() + "&type=rain&value=false");
      Serial.println("Regen: Nein");
    }
    else
    {
      m.sender.begin("http://syntex.local:1710/devices?mac=" + WiFi.macAddress() + "&type=rain&value=true");
      Serial.println("Regen: Ja");
    }

    m.sender.GET();
    m.sender.end();
  }
}
