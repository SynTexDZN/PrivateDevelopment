#include "light.h"
#include "accessory.h"
#include "Arduino.h"
#include <Wire.h>
#include <BH1750.h>

Accessory lightAccessory;
BH1750 lightMeter;

Light::Light() {}

void Light::SETUP(String ip, String port, int interval, boolean led)
{
  lightAccessory.SETUP("1.1.2", interval, "[]", ip, port, led);

  Wire.begin();
  
  lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
  
  activated = true;
}

void Light::UPDATE(boolean force)
{
  unsigned long currentMillis = millis();

  if(force || currentMillis - previousMillis >= lightAccessory.Interval)
  {
    previousMillis = currentMillis;   

    float lighttmp = lightMeter.readLightLevel();

    Serial.print("Licht: " + String(lighttmp) + " lux");
      
    Serial.println(" ( " + String(lighttmp != light ? "X" : "O") + " )");

    if(lighttmp != light)
    {
      light = lighttmp;

      lightAccessory.safeFetch(lightAccessory.BridgeIP + ":" + String(lightAccessory.WebhookPort) + "/devices?id=" + WiFi.macAddress() + "&type=light&value=" + String(light), lightAccessory.Interval, false);
    }
  }
}
