#include "climate.h"
#include "accessory.h"
#include "Arduino.h"
#include <DHT.h>

Accessory climateAccessory;
DHT dht = DHT(2, DHT11);

Climate::Climate(int Pin)
{
  this -> Pin = Pin;

  dht = DHT(Pin, DHT11);

  tempPuffer = 0.5;
  humPuffer = 1;
}

void Climate::SETUP(String ip, String port, int interval, boolean led)
{
  climateAccessory.SETUP("1.3.0", interval, "[]", ip, port, led);

  dht.begin();

  activated = true;
}

void Climate::UPDATE(boolean force)
{
  unsigned long currentMillis = millis();

  if(force || currentMillis - previousMillis >= climateAccessory.Interval)
  {
    previousMillis = currentMillis;   

    float humtmp = dht.readHumidity();
    float temptmp = dht.readTemperature();

    if(isnan(humtmp) || isnan(temptmp) || humtmp < 0 || humtmp > 100 || temptmp < -100 || temptmp > 140)
    {
      Serial.println("Fehler beim Lesen des DHT Sensors!");
    }
    else
    {
      temptmp -= 2.6;
      
      Serial.print("Temperatur: " + String(temptmp) + " - Feuchtigkeit: " + String((int)humtmp) + "%");
      
      Serial.println(" ( " + String(temp - temptmp >= tempPuffer || temp - temptmp <= -tempPuffer ? "X" : "O") + " | " + String(humtmp != hum ? "X" : "O") + " )");

      tempExact = temptmp;
      humExact = humtmp;
      
      if(temp - temptmp >= tempPuffer || temp - temptmp <= -tempPuffer)
      {
        temp = temptmp;

        climateAccessory.safeFetch(climateAccessory.BridgeIP + ":" + String(climateAccessory.WebhookPort) + "/devices?id=" + WiFi.macAddress() + "&type=temperature&value=" + String(temp), climateAccessory.Interval, false);
      }

      if(humtmp != humPrevious && (hum - humtmp == 1 || hum - humtmp == 0 || hum - humtmp == -1))
      {
        humPrevious = humtmp;
      }
      else if((humtmp == humPrevious && hum != humtmp) || hum - humtmp > 1 || hum - humtmp < -1)
      {
        hum = humtmp;
        
        climateAccessory.safeFetch(climateAccessory.BridgeIP + ":" + String(climateAccessory.WebhookPort) + "/devices?id=" + WiFi.macAddress() + "&type=humidity&value=" + String((int)hum), climateAccessory.Interval, false);
      }
    }
  }
}

void Climate::setDHT22()
{
  dht = DHT(Pin, DHT22);

  tempPuffer = 0.2;
}
