#include "relais.h"
#include "accessory.h"
#include "Arduino.h"

Accessory relaisAccessory;

Relais::Relais(int Pin)
{
  this -> Pin = Pin;
}

void Relais::SETUP(String ip, String port, boolean led, ESP8266WebServer &server)
{
  relaisAccessory.SETUP("motion", "1.0.1", 10000, "[]", ip, port, led);

  pinMode(Pin, OUTPUT);
  
  server.on("/relais", [&] ()
  {
    if(server.hasArg("value"))
    {
      digitalWrite(Pin, server.arg("value") == "true" ? HIGH : LOW);
    }
    else
    {
      digitalWrite(Pin, digitalRead(Pin) ? LOW : HIGH);
    }

    boolean relais = digitalRead(Pin);

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
