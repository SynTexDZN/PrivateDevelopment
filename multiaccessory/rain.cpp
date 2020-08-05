#include "rain.h"
#include "accessory.h"
#include "Arduino.h"

Accessory rainAccessory;

Rain::Rain() {}

void Rain::SETUP(String ip, String port, int interval, String events, boolean led)
{
  rainAccessory.SETUP("rain", "1.0.0", interval, events, ip, port, led);
  
  activated = true;
}

void Rain::UPDATE(boolean force)
{
  boolean raintmp = digitalRead(16);
    
  if(force || raintmp != rain)
  {
    rain = raintmp;

    Serial.print("Regen: ");
    Serial.println(rain ? "Nein" : "Ja");

    rainAccessory.safeFetch(rainAccessory.BridgeIP + ":" + String(rainAccessory.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&type=rain&value=" + (rain ? "false" : "true"), rainAccessory.Interval, false);
  }
}
