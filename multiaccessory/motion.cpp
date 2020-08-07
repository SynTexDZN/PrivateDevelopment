#include "motion.h"
#include "accessory.h"
#include "Arduino.h"

Accessory motionAccessory;

Motion::Motion() {}

void Motion::SETUP(String ip, String port, int interval, boolean led)
{
  motionAccessory.SETUP("motion", "1.0.1", interval, "[]", ip, port, led);
  
  activated = true;
}

void Motion::UPDATE(boolean force)
{
  unsigned long currentMillis = millis();

  boolean motiontmp = digitalRead(14);

  if(motiontmp)
  {
    previousMillis = currentMillis;   
  }

  if(motiontmp != motion && (force || motiontmp || currentMillis - previousMillis >= motionAccessory.Interval))
  {
    motion = motiontmp;

    Serial.print("Bewegung: ");
    Serial.println(motion ? "Ja" : "Nein");

    int response = motionAccessory.safeFetch(motionAccessory.BridgeIP + ":" + String(motionAccessory.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&type=motion&value=" + (motion ? "true" : "false"), 10000, false);

    if(response == HTTP_CODE_OK && motionAccessory.LED)
    {
      digitalWrite(LED_BUILTIN, motion ? LOW : HIGH);
    }
  }
}
