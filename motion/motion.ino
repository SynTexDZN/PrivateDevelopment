#include "main.h"

SynTexMain m;

boolean motion;
unsigned long previousMillis;

void setup()
{
  if(m.SETUP("motion", "3.5.0", 5000) && m.checkConnection())
  {
    previousMillis = -m.Interval;
    
    getMotion();
  }
}

void loop()
{
  m.LOOP();

  if(m.checkConnection())
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
      m.sender.begin("http://syntex.local:1710/devices?mac=" + WiFi.macAddress() + "&value=true");

      if(m.LED)
      {
        digitalWrite(LED_BUILTIN, LOW);
      }
      
      Serial.println("Bewegung: Ja");
    }
    else
    {
      m.sender.begin("http://syntex.local:1710/devices?mac=" + WiFi.macAddress() + "&value=false");

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
