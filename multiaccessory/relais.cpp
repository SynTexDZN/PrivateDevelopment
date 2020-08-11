#include "relais.h"
#include "accessory.h"
#include "Arduino.h"

Accessory relaisAccessory;

Relais::Relais() {}

void Relais::SETUP(String ip, String port, boolean led, ESP8266WebServer &server)
{
  relaisAccessory.SETUP("motion", "1.0.1", 10000, "[]", ip, port, led);

  pinMode(2, OUTPUT);
  
  server.on("/switch", [&] ()
  {
    if(server.hasArg("value"))
    {
      digitalWrite(2, server.arg("value") == "true" ? HIGH : LOW);
    }
    else
    {
      digitalWrite(2, digitalRead(2) ? LOW : HIGH);
    }

    boolean relais = digitalRead(2);

    Serial.print("Relais: ");
    Serial.println(relais ? "An" : "Aus");

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", relais ? "true" : "false");

    if(relaisAccessory.LED)
    {
      digitalWrite(LED_BUILTIN, relais ? LOW : HIGH);
    }
  });

  relaisAccessory.safeFetch(relaisAccessory.BridgeIP + ":" + String(relaisAccessory.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&type=relais&value=false", 10000, false);
  
  activated = true;
}
