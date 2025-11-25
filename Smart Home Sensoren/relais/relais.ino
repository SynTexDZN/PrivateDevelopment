#include "main.h"

SynTexMain m;

void setup()
{
  if(m.SETUP("relais", "4.3.1", 0, "[]") && m.checkConnection())
  {
    pinMode(5, OUTPUT);
  
    m.server.on("/switch", []
    {
      if(m.server.hasArg("value"))
      {
        digitalWrite(5, m.server.arg("value") == "true" ? HIGH : LOW);
      }
      else
      {
        digitalWrite(5, digitalRead(5) ? LOW : HIGH);
      }

      boolean relais = digitalRead(5);

      Serial.print("Relais: ");
      Serial.println(relais ? "An" : "Aus");

      m.server.sendHeader("Access-Control-Allow-Origin", "*");
      m.server.send(200, "text/plain", relais ? "true" : "false");
  
      if(m.LED)
      {
        digitalWrite(LED_BUILTIN, relais ? LOW : HIGH);
      }
    });

    m.safeFetch(m.BridgeIP + ":" + String(m.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&value=" + (digitalRead(5) ? "true" : "false"), 10, false);
  }
}

void loop()
{
  m.LOOP();
}
