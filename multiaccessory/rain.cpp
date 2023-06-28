#include "rain.h"
#include "accessory.h"
#include "Arduino.h"

Accessory rainAccessory;

Rain::Rain(int Pin)
{
  this -> Pin = Pin;
}

void Rain::SETUP(String ip, String port, int interval, boolean led)
{
  rainAccessory.SETUP("1.0.2", interval, "[]", ip, port, led);
  
  activated = true;
}

void Rain::UPDATE(boolean force)
{
  boolean raintmp = digitalRead(Pin);
    
  if(force || raintmp != rain)
  {
    unsigned long currentMillis = millis();

    if(raintmp)
    {
      previousMillis = currentMillis;
    }

    if(raintmp != rain && (force || raintmp || currentMillis - previousMillis >= rainAccessory.Interval))
    {
      rain = raintmp;

      Serial.print("Regen: ");
      Serial.println(rain ? "Nein" : "Ja");

      rainAccessory.safeFetch(rainAccessory.BridgeIP + ":" + String(rainAccessory.WebhookPort) + "/devices?id=" + WiFi.macAddress() + "&type=rain&value=" + (rain ? "false" : "true"), rainAccessory.Interval, false);
    }
  }
}
