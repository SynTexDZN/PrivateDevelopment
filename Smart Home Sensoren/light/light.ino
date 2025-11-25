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
  if(m.SETUP("light", "4.3.3", 10000, "[]") && m.checkConnection())
  {
    Wire.begin();
    lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE_2);

    previousMillis = -m.Interval;

    if(m.Active)
    {
      getLight();

      rain = digitalRead(2);
      
      m.safeFetch(m.BridgeIP + ":" + String(m.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&type=rain&value=" + (rain ? "false" : "true"), m.Interval, false);
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

      Serial.println("Licht: " + String(light) + " lux");

      m.safeFetch(m.BridgeIP + ":" + String(m.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&type=" + m.Type + "&value=" + String(light), m.Interval, false);

      for(int i = 0; i < m.EventsNegative; i++)
      {
        if(light < m.EventControlNegative[i] && !m.EventLockNegative[i])
        {
          int response = m.safeFetch(m.BridgeIP + ":" + String(m.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&event=" + i, m.Interval, false);

          if(response == HTTP_CODE_OK)
          {
            for(int j = 0; j < m.EventsPositive; j++)
            {
              m.EventLockPositive[j] = false;
            }
            
            m.EventLockNegative[i] = true;

            Serial.println("( " + String(i) + " ) Scene wird aktiviert!");
          }
          else
          {
            Serial.println("( " + String(i) + " ) Scene konnte nicht aktiviert werden!");
          }
        }
        
        for(int i = 0; i < m.EventsPositive; i++)
        {  
          if(light > m.EventControlPositive[i] && !m.EventLockPositive[i])
          {
            int response = m.safeFetch(m.BridgeIP + ":" + String(m.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&event=" + String(i + m.EventsNegative), m.Interval, false);

            if(response == HTTP_CODE_OK)
            {
              for(int j = 0; j < m.EventsNegative; j++)
              {
                m.EventLockNegative[j] = false;
              }
              
              m.EventLockPositive[i] = true;
              
              Serial.println("( " + String(i + m.EventsNegative) + " ) Scene wird aktiviert!");
            }
            else
            {
              Serial.println("( " + String(i + m.EventsNegative) + " ) Scene konnte nicht aktiviert werden!");
            }
          }
        }
      }
    }
  }
}

void getRain()
{
  boolean raintmp = digitalRead(2);
    
  if(raintmp != rain)
  {
    rain = raintmp;

    Serial.print("Regen: ");
    Serial.println(rain ? "Nein" : "Ja");

    m.safeFetch(m.BridgeIP + ":" + String(m.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&type=rain&value=" + (rain ? "false" : "true"), m.Interval, false);
  }
}
