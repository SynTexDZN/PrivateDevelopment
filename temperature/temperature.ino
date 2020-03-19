#include "main.h"
#include <DHT.h>

SynTexMain m;
DHT dht(2, DHT11);

float temp;
float hum;
boolean *ScenesNegative;
boolean *ScenesPositive;
unsigned long previousMillis;

void setup()
{
  if(m.SETUP("temperature", "3.4.0", 10000) && m.checkConnection())
  {
    dht.begin();

    previousMillis = -m.Interval;

    ScenesNegative = new boolean [m.SceneCountNegative];
    
    for(int i = 0; i < m.SceneCountNegative; i++)
    {
      ScenesNegative[i] = false;
    }

    ScenesPositive = new boolean [m.SceneCountPositive];
    
    for(int i = 0; i < m.SceneCountPositive; i++)
    {
      ScenesPositive[i] = false;
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

        for(int i = 0; i < m.SceneCountNegative; i++)
        {
          if(temp < m.SceneControlNegative[i] && !ScenesNegative[i])
          {
            m.sender.begin("http://syntex.local:1710/devices?mac=" + WiFi.macAddress() + "&event=" + i);
            m.sender.GET();
            m.sender.end();
            
            for(int j = 0; j < m.SceneCountPositive; j++)
            {
              ScenesPositive[j] = false;
            }
            
            ScenesNegative[i] = true;
    
            Serial.println("( " + String(i) + " ) Scene wird aktiviert!");
          }
          
          for(int i = 0; i < m.SceneCountPositive; i++)
          {  
            if(temp > m.SceneControlPositive[i] && !ScenesPositive[i])
            {
              m.sender.begin("http://syntex.local:1710/devices?mac=" + WiFi.macAddress() + "&event=" + String(i + m.SceneCountNegative));
              m.sender.GET();
              m.sender.end();
              
              for(int j = 0; j < m.SceneCountNegative; j++)
              {
                ScenesNegative[j] = false;
              }
              
              ScenesPositive[i] = true;
              
              Serial.println("( " + String(i + m.SceneCountNegative) + " ) Scene wird aktiviert!");
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
