#include "main.h"

SynTexMain m;

boolean motion;
unsigned long previousMillis;

void setup()
{
  if(m.SETUP("motion", "4.3.1", 5000, "[]") && m.checkConnection())
  {
    previousMillis = -m.Interval;

    if(m.Active)
    {
      motion = digitalRead(14);
    
      m.safeFetch(m.BridgeIP + ":" + String(m.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&value=" + (motion ? "true" : "false"), 10, false);
    }
  }
}

void loop()
{
  m.LOOP();

  if(m.checkConnection() && m.Active)
  {
    getMotion();
  }
}

void getMotion()
{
  unsigned long currentMillis = millis();
 
  boolean motiontmp = digitalRead(14);

  if(motiontmp)
  {
    previousMillis = currentMillis;   
  }

  if(motiontmp != motion && (motiontmp || currentMillis - previousMillis >= m.Interval))
  {
    motion = motiontmp;

    Serial.print("Bewegung: ");
    Serial.println(motion ? "Ja" : "Nein");

    int response = m.safeFetch(m.BridgeIP + ":" + String(m.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&value=" + (motion ? "true" : "false"), 10, false);

    if(response == HTTP_CODE_OK && m.LED)
    {
      digitalWrite(LED_BUILTIN, motion ? LOW : HIGH);
    }
  }
}
