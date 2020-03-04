#include "main.h"

SynTexMain m;

void setup()
{
  m.SETUP("airquality", "3.0.0", 10000);

  //m.sender.begin("http://syntex.local:1710/devices?mac=" + WiFi.macAddress() + "&type=" + m.Type + "&value=false");
}

void loop()
{
  m.LOOP();

  if(m.checkConnection())
  {
    getAirQuality();
  }
}

float quality;
unsigned long previousMillis = -500;

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

    t1=(analogRead(A0)/1024.0) * 3300;
    Serial.print("smoke: ");
    Serial.println(t1);

    t2=(t1/10);
    Serial.print("celc: ");
    Serial.println(t2);

    t3=((t2 * 9)/5 + 32);
    Serial.print("fhrn: ");
    Serial.println(t3);
    
    if(qualitytmp != quality)
    {
      quality = qualitytmp;
      //sender.begin("http://192.168.188.121:51828/?accessoryId=sensor7&value=" + String(quality));
      //sender.GET();
      //sender.end();
    }

    Serial.println("Luftqualität: " + String(quality));
  }
}
