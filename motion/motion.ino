#include "main.h"

SynTexMain m;

boolean motion;
unsigned long previousMillis;

void setup()
{
  if(m.SETUP("motion", "4.2.0", 5000, "[]") && m.checkConnection())
  {
    previousMillis = -m.Interval;

    if(m.Active)
    {
      getMotion();
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

    if(motion)
    {
      m.sender.begin(m.BridgeIP + ":" + String(m.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&value=true");

      if(m.LED)
      {
        digitalWrite(LED_BUILTIN, LOW);
      }
      
      Serial.println("Bewegung: Ja");
    }
    else
    {
      m.sender.begin(m.BridgeIP + ":" + String(m.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&value=false");

      if(m.LED)
      {
        digitalWrite(LED_BUILTIN, HIGH);
      }
      
      Serial.println("Bewegung: Nein");
    }
    
    m.sender.GET();
    m.sender.end();
  }
}
