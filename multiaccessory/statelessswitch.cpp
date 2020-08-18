#include "statelessswitch.h"
#include "accessory.h"
#include "Arduino.h"

Accessory statelessswitchAccessory;

StatelessSwitch::StatelessSwitch(int Pin)
{
  this -> Pin = Pin;
}

void StatelessSwitch::SETUP(String ip, String port, String events, boolean led, ESP8266WebServer &server)
{
  statelessswitchAccessory.SETUP("motion", "1.0.0", 10000, events, ip, port, led);

  pinMode(Pin, OUTPUT);
  
  server.on("/statelessswitch", [&] ()
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

    if(statelessswitchAccessory.LED)
    {
      digitalWrite(LED_BUILTIN, relais ? LOW : HIGH);
    }
  });

  button = new boolean[statelessswitchAccessory.EventsPositive];
    
  for(int i = 0; i < statelessswitchAccessory.EventsPositive; i++)
  {
    button[i] = false;
  }
  
  activated = true;
}

void StatelessSwitch::UPDATE(boolean force)
{
  for(int i = 0; i < statelessswitchAccessory.EventsPositive; i++)
  {
    boolean buttontmp = digitalRead(statelessswitchAccessory.EventControlPositive[i]);
    
    if(buttontmp && !button[i] && !statelessswitchAccessory.EventLockPositive[i])
    {
      button[i] = true;
      statelessswitchAccessory.EventLockPositive[i] = true;
    }
    else if(buttontmp && button[i] && !statelessswitchAccessory.EventLockPositive[i])
    {
      button[i] = false;
      statelessswitchAccessory.EventLockPositive[i] = true;
    }
    
    if(!buttontmp && statelessswitchAccessory.EventLockPositive[i])
    {
      if(!buttontmp)
      {
        statelessswitchAccessory.EventLockPositive[i] = false;
      }
  
      Serial.println("Schalter Nr. " + String(i + 1) + ": " + (button[i] ? "An" : "Aus"));
  
      int response = statelessswitchAccessory.safeFetch(statelessswitchAccessory.BridgeIP + ":" + statelessswitchAccessory.WebhookPort + "/devices?mac=" + WiFi.macAddress() + "&event=" + i + "&value=" + (button[i] ? 0 : 1), 10000, false);
  
      if(response == HTTP_CODE_OK && statelessswitchAccessory.LED)
      {
        if(statelessswitchAccessory.EventsPositive == 1)
        {
          digitalWrite(LED_BUILTIN, button[i] ? HIGH : LOW);
        }
        else
        {
          digitalWrite(LED_BUILTIN, LOW);
          delay(button[i] ? 1000 : 250);
          digitalWrite(LED_BUILTIN, HIGH);
        }
      }
    }
  }
}
