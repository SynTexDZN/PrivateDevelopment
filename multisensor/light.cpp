#include "light.h"
#include "accessory.h"
#include "Arduino.h"
#include <Wire.h>
#include <BH1750.h>

Accessory lightAccessory;
BH1750 lightMeter;

Light::Light() { }

void Light::SETUP(String ip, String port, int interval, String events)
{
  lightAccessory.SETUP("light", "5.0.0", interval, events, ip, port);

  Wire.begin();
  
  lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE_2);
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

      lightAccessory.safeFetch(lightAccessory.BridgeIP + ":" + String(lightAccessory.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&type=light&value=" + String(light), lightAccessory.Interval, false);

      lightAccessory.updateScenes(light);
    }
  }
}
