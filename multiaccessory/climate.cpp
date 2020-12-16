#include "climate.h"
#include "accessory.h"
#include "Arduino.h"
#include <DHT.h>

Accessory climateAccessory;
DHT dht(2, DHT11);

Climate::Climate(int Pin)
{
  this -> Pin = Pin;
}

void Climate::SETUP(String ip, String port, int interval, boolean led)
{
  climateAccessory.SETUP("1.1.2", interval, "[]", ip, port, led);

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

    if(isnan(humtmp) || isnan(temptmp))
    {
      Serial.println("Fehler beim Lesen des DHT Sensors!");
    }
    else
    {
      Serial.print("Temperatur: " + String(temptmp) + " - Feuchtigkeit: " + String((int)humtmp) + "%");
      
      Serial.println(" ( " + String(temp - temptmp >= 0.5 || temp - temptmp <= -0.5 ? "X" : "O") + " | " + String(humtmp != hum ? "X" : "O") + " )");
      
      if(temp - temptmp >= 0.5 || temp - temptmp <= -0.5)
      {
        temp = temptmp;

        climateAccessory.safeFetch(climateAccessory.BridgeIP + ":" + String(climateAccessory.WebhookPort) + "/devices?id=" + WiFi.macAddress() + "&type=temperature&value=" + String(temp), climateAccessory.Interval, false);
      }
      
      if(humtmp != hum)
      {
        hum = humtmp;

        climateAccessory.safeFetch(climateAccessory.BridgeIP + ":" + String(climateAccessory.WebhookPort) + "/devices?id=" + WiFi.macAddress() + "&type=humidity&value=" + String((int)hum), climateAccessory.Interval, false);
      }
    }
  }
}
