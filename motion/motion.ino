#include "main.h"

SynTexMain m;

void setup()
{
  m.SETUP("motion", "3.0.0", 5000);

  m.sender.begin("http://syntex.local:1710/devices?mac=" + WiFi.macAddress() + "&type=" + m.Type + "&value=false");
  m.sender.GET();
  m.sender.end();
}

void loop()
{
  m.LOOP();

  if(m.checkConnection())
  {
    getMotion();
  }
}

boolean motion;
unsigned long previousMillis = 0;

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
      m.sender.begin("http://syntex.local:1710/devices?mac=" + WiFi.macAddress() + "&type=" + m.Type + "&value=true");

      if(m.LED)
      {
        digitalWrite(LED_BUILTIN, LOW);
      }
      
      Serial.println("Bewegung: Ja");
    }
    else
    {
      m.sender.begin("http://syntex.local:1710/devices?mac=" + WiFi.macAddress() + "&type=" + m.Type + "&value=false");

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
