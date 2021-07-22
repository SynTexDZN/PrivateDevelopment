#include "temperature.h"
#include "accessory.h"
#include "Arduino.h"
#include <OneWire.h>
#include <DallasTemperature.h>

Accessory temperatureAccessory;
OneWire oneWire(2);
DallasTemperature sensor(&oneWire);

Temperature::Temperature(int Pin)
{
  this -> Pin = Pin;

  oneWire = OneWire(Pin);
  sensor = DallasTemperature(&oneWire);
}

void Temperature::SETUP(String ip, String port, int interval, boolean led)
{
  temperatureAccessory.SETUP("1.0.0", interval, "[]", ip, port, led);

  sensor.begin();

  activated = true;
}

void Temperature::UPDATE(boolean force)
{
  unsigned long currentMillis = millis();

  if(force || currentMillis - previousMillis >= temperatureAccessory.Interval)
  {
    previousMillis = currentMillis;   

    sensor.requestTemperatures(); 

    float temptmp = sensor.getTempCByIndex(0);

    if(temptmp == -127)
    {
      Serial.println("Fehler beim Lesen des Temperatur Sensors!");
    }
    else
    {
      Serial.print("Temperatur: " + String(temptmp));
      
      Serial.println(" ( " + String(temp - temptmp >= 0.5 || temp - temptmp <= -0.5 ? "X" : "O") + " )");

      tempExact = temptmp;
      
      if(temp - temptmp >= 0.5 || temp - temptmp <= -0.5)
      {
        temp = temptmp;

        temperatureAccessory.safeFetch(temperatureAccessory.BridgeIP + ":" + String(temperatureAccessory.WebhookPort) + "/devices?id=" + WiFi.macAddress() + "&type=temperature&value=" + String(temp), temperatureAccessory.Interval, false);
      }
    }
  }
}
