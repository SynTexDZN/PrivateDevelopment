#include "main.h"
#include <DHT.h>

SynTexMain m;
DHT dht(2, DHT11);

float temp;
float hum;
unsigned long previousMillis;

void setup()
{
  if(m.SETUP("temperature", "4.3.1", 10000, "[]") && m.checkConnection())
  {
    dht.begin();

    previousMillis = -m.Interval;

    if(m.Active)
    {
      getTempHum();
    }
  }
}

void loop()
{
  m.LOOP();

  if(m.checkConnection() && m.Active)
  {
    getTempHum();
  }
}

void getTempHum()
{
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis >= m.Interval)
  {
    previousMillis = currentMillis;   
 
    float humtmp = dht.readHumidity();
    float temptmp = dht.readTemperature();

    if(isnan(humtmp) || isnan(temptmp))
    {
      Serial.println("Fehler beim Lesen vom DHT Sensor!");
    }
    else
    {
      Serial.println("Temperatur: " + String(temp) + " - Humidity: " + String((int)hum) + "%");
      
      if(temp - temptmp >= 0.5 || temp - temptmp <= -0.5)
      {
        temp = temptmp;

        m.safeFetch(m.BridgeIP + ":" + String(m.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&type=" + m.Type + "&value=" + String(temp), m.Interval, false);

        for(int i = 0; i < m.EventsNegative; i++)
        {
          if(temp < m.EventControlNegative[i] && !m.EventLockNegative[i])
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
            if(temp > m.EventControlPositive[i] && !m.EventLockPositive[i])
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
      
      if(humtmp != hum)
      {
        hum = humtmp;

        m.safeFetch(m.BridgeIP + ":" + String(m.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&type=humidity&value=" + String((int)hum), m.Interval, false);
      }      
    }
  }
}
