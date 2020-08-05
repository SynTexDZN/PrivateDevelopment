#include "relais.h"
#include "accessory.h"
#include "Arduino.h"

Accessory relaisAccessory;

Relais::Relais() {}

void Relais::SETUP(String ip, String port, String events, boolean led, ESP8266WebServer &server)
{
  relaisAccessory.SETUP("motion", "1.0.0", 10000, events, ip, port, led);

  pinMode(5, OUTPUT);
  
  server.on("/switch", [&] ()
  {
    if(server.hasArg("value"))
    {
      digitalWrite(5, server.arg("value") == "true" ? HIGH : LOW);
    }
    else
    {
      digitalWrite(5, digitalRead(5) ? LOW : HIGH);
    }

    boolean relais = digitalRead(5);

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
