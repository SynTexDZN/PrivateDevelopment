#include "main.h"
#include <DHT.h>

SynTexMain m;
DHT dht(2, DHT11);

void setup()
{
  if(m.SETUP("temperature", "3.2.0", 10000) && m.checkConnection())
  {
    dht.begin();
    
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

float temp;
float hum;
unsigned long previousMillis = 0;

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
