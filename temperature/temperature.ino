#include "main.h"
#include <DHT.h>

SynTexMain m;
DHT dht(2, DHT11);

void setup()
{
  m.SETUP("temperature", "3.0.0", 10000);

  dht.begin();
}

void loop()
{
  m.LOOP();

  if(m.checkConnection())
  {
    getTemperature();
  }
}

float temp;
float hum;
unsigned long previousMillis = -10000000;

void getTemperature()
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
      /*
      if((hum - humtmp > 5 || hum - humtmp < -5) || (temp - temptmp > 0.5 || temp - temptmp < -0.5))
      {
        interval = 2000;
      }
      else if(hum - humtmp == 0 && temp - temptmp == 0)
      {
        interval = 30000;
      }
      else if(hum - humtmp < 2 && hum - humtmp > -2 && temp - temptmp < 0.2 && temp - temptmp > -0.2)
      {
        interval = 20000;
      }
      else if(hum - humtmp < 5 && hum - humtmp > -5 && temp - temptmp < 0.5 && temp - temptmp > -0.5)
      {
        interval = 5000;
      }
      */
      if(temptmp != temp)
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
