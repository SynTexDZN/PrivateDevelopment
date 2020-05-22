#include "main.h"
#include <Wire.h>
#include <BH1750.h>

SynTexMain m;
BH1750 lightMeter;

float light;
boolean rain;
unsigned long previousMillis;

void setup()
{
  if(m.SETUP("light", "4.3.0", 10000, "[]") && m.checkConnection())
  {
    Wire.begin();
    lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE_2);

    previousMillis = -m.Interval;

    if(m.Active)
    {
      getLight();
      getRain();
    }
  }
}

void loop()
{
  m.LOOP();

  if(m.checkConnection() && m.Active)
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
      m.sender.begin(m.BridgeIP + ":" + String(m.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&type=" + m.Type + "&value=" + String(light));
      m.sender.GET();
      m.sender.end();

      for(int i = 0; i < m.EventsNegative; i++)
      {
        if(light < m.EventControlNegative[i] && !m.EventLockNegative[i])
        {
          m.sender.begin(m.BridgeIP + ":" + String(m.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&event=" + i);
          m.sender.GET();
          m.sender.end();
          
          for(int j = 0; j < m.EventsPositive; j++)
          {
            m.EventLockPositive[j] = false;
          }
          
          m.EventLockNegative[i] = true;
  
          Serial.println("( " + String(i) + " ) Scene wird aktiviert!");
        }
        
        for(int i = 0; i < m.EventsPositive; i++)
        {  
          if(light > m.EventControlPositive[i] && !m.EventLockPositive[i])
          {
            m.sender.begin(m.BridgeIP + ":" + String(m.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&event=" + String(i + m.EventsNegative));
            m.sender.GET();
            m.sender.end();
            
            for(int j = 0; j < m.EventsNegative; j++)
            {
              m.EventLockNegative[j] = false;
            }
            
            m.EventLockPositive[i] = true;
            
            Serial.println("( " + String(i + m.EventsNegative) + " ) Scene wird aktiviert!");
          }
        }
      }
    }
    
    Serial.println("Licht: " + String(light) + " lux");
  }
}

void getRain()
{
  boolean raintmp = digitalRead(2);
    
  if(raintmp != rain)
  {
    rain = raintmp;

    if(rain)
    {
      m.sender.begin(m.BridgeIP + ":" + String(m.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&type=rain&value=false");
      Serial.println("Regen: Nein");
    }
    else
    {
      m.sender.begin(m.BridgeIP + ":" + String(m.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&type=rain&value=true");
      Serial.println("Regen: Ja");
    }

    m.sender.GET();
    m.sender.end();
  }
}
