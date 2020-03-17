#include "main.h"
#include <DHT.h>

SynTexMain m;
DHT dht(2, DHT11);

float temp;
float hum;
boolean *Scenes;
unsigned long previousMillis;

void setup()
{
  if(m.SETUP("temperature", "3.3.0", 10000) && m.checkConnection())
  {
    dht.begin();

    previousMillis = -m.Interval;

    Scenes = new boolean [m.SceneCount];
    
    for(int i = 0; i < m.SceneCount; i++)
    {
      Scenes[i] = false;
    }
    
    getTempHum();
  }
}

void loop()
{
  m.LOOP();

  if(m.checkConnection())
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

    if (isnan(humtmp) || isnan(temptmp))
    {
      Serial.println("Fehler beim Lesen vom DHT Sensor!");
    }
    else
    {
      if(temp - temptmp >= 0.5 || temp - temptmp <= -0.5)
      {
        temp = temptmp;
        m.sender.begin("http://syntex.local:1710/devices?mac=" + WiFi.macAddress() + "&type=" + m.Type + "&value=" + String(temp));
        m.sender.GET();
        m.sender.end();

        if(m.SceneControl)
        {
          for(int i = 0; i < m.SceneCount; i++)
          {
            if(m.SceneControl[i] < 0)
            {
              if(temp < -m.SceneControl[i] && !Scenes[i])
              {
                m.sender.begin("http://syntex.local:1710/devices?mac=" + WiFi.macAddress() + "&event=" + i);
                m.sender.GET();
                m.sender.end();
                
                for(int j = 0; j < m.SceneCount; j++)
                {
                  if(m.SceneControl[j] >= 0)
                  {
                    Scenes[j] = false;  
                  }
                }
                
                Scenes[i] = true;
        
                Serial.println("( " + String(i) + " ) Scene wird aktiviert!");
              }
            }
            else
            {              
              if(temp > m.SceneControl[i] && !Scenes[i])
              {
                m.sender.begin("http://syntex.local:1710/devices?mac=" + WiFi.macAddress() + "&event=" + i);
                m.sender.GET();
                m.sender.end();
                
                for(int j = 0; j < m.SceneCount; j++)
                {
                  if(m.SceneControl[j] < 0)
                  {
                    Scenes[j] = false;  
                  }
                }
                
                Scenes[i] = true;
                
                Serial.println("( " + String(i) + " ) Reset-Scene wird aktiviert!");
                
              }
            }
          }  
        }
      }
      
      if(humtmp != hum)
      {
        hum = humtmp;
        m.sender.begin("http://syntex.local:1710/devices?mac=" + WiFi.macAddress() + "&type=humidity&value=" + String(hum));
        m.sender.GET();
        m.sender.end();
      }      
      
      Serial.println("Temperatur: " + String(temp) + " - Humidity: " + String((int)hum) + "%");
    }
  }
}
