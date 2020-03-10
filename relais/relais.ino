#include "main.h"

SynTexMain m;

void setup()
{
  if(m.SETUP("relais", "3.2.0", 0) && m.checkConnection())
  {
    m.sender.begin("http://syntex.local:1710/devices?mac=" + WiFi.macAddress() + "&value=false");
    m.sender.GET();
    m.sender.end();
  
    pinMode(5, OUTPUT);
  
    m.server.on("/switch", []
    {
      if(m.server.hasArg("value"))
      {
        (m.server.arg("value") == "true") ? digitalWrite(5, HIGH) : digitalWrite(5, LOW);
      }
      else
      {
        digitalRead(5) ? digitalWrite(5, LOW) : digitalWrite(5, HIGH);
      }
  
      if(digitalRead(5))
      {
        Serial.println("Relais: An");
  
        if(m.LED)
        {
          digitalWrite(LED_BUILTIN, LOW);
        }
      }
      else
      {
        Serial.println("Relais: Aus");
  
        if(m.LED)
        {
          digitalWrite(LED_BUILTIN, HIGH);
        }
      }
  
      m.server.sendHeader("Access-Control-Allow-Origin", "*");
      m.server.send(200, "text/plain", String(digitalRead(5)));
    });
  }
}

void loop()
{
  m.LOOP();
}
