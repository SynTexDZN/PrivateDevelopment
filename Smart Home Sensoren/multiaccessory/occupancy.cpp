#include "occupancy.h"
#include "accessory.h"
#include "Arduino.h"

Accessory occupancyAccessory;

Occupancy::Occupancy(int Pin)
{
  this -> Pin = Pin;
}

void Occupancy::SETUP(String ip, String port, int interval, boolean led)
{
  occupancyAccessory.SETUP("1.0.0", interval, "[]", ip, port, led);
  
  activated = true;
}

void Occupancy::UPDATE(boolean force)
{
  unsigned long currentMillis = millis();

  boolean occupancytmp = digitalRead(Pin);

  if(occupancytmp)
  {
    previousMillis = currentMillis;
  }

  if(occupancytmp != occupancy && (force || occupancytmp || currentMillis - previousMillis >= occupancyAccessory.Interval))
  {
    occupancy = occupancytmp;

    Serial.print("Bewegung: ");
    Serial.println(occupancy ? "Ja" : "Nein");

    String response = occupancyAccessory.safeFetch(occupancyAccessory.BridgeIP + ":" + String(occupancyAccessory.WebhookPort) + "/devices?id=" + WiFi.macAddress() + "&type=occupancy&value=" + (occupancy ? "true" : "false"), occupancyAccessory.Interval, false)[0];

    if(response == "OK" && occupancyAccessory.LED)
    {
      digitalWrite(LED_BUILTIN, occupancy ? LOW : HIGH);
    }
  }
}
