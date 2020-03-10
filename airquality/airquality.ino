#include "main.h"

SynTexMain m;

float quality;
unsigned long previousMillis;

void setup()
{
  if(m.SETUP("airquality", "3.2.0", 10000) && m.checkConnection())
  {
    previousMillis = -m.Interval;
    
    getAirQuality();
  }
}

void loop()
{
  m.LOOP();

  if(m.checkConnection())
  {
    getAirQuality();
  }
}

void getAirQuality()
{
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis >= m.Interval)
  {
    previousMillis = currentMillis;   
 
    float qualitytmp = 1.0 + analogRead(A0) / 256.0;

    float t1;
    float t2;
    float t3;

    t1 = (analogRead(A0) / 1024.0) * 3300;
    Serial.print("Smoke: ");
    Serial.println(t1);

    t2 = (t1 / 10);
    Serial.print("Celc: ");
    Serial.println(t2);

    t3 = ((t2 * 9) / 5 + 32);
    Serial.print("Fhrn: ");
    Serial.println(t3);
    
    if(qualitytmp != quality)
    {
      quality = qualitytmp;
      m.sender.begin("http://syntex.local:1710/devices?mac=" + WiFi.macAddress() + "&value=" + String(quality));
      m.sender.GET();
      m.sender.end();
    }

    Serial.println("Luftqualität: " + String(quality));
  }
}
