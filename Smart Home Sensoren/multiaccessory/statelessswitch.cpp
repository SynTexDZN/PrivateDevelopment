#include "statelessswitch.h"
#include "accessory.h"
#include "Arduino.h"

Accessory statelessswitchAccessory;

StatelessSwitch::StatelessSwitch(int Pin)
{
  this -> Pin = Pin;
}

void StatelessSwitch::SETUP(String ip, String port, String buttons, boolean led, ESP8266WebServer &server)
{
  statelessswitchAccessory.SETUP("1.0.1", 10000, buttons, ip, port, led);

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

  button = new boolean[statelessswitchAccessory.ButtonCount];
    
  for(int i = 0; i < statelessswitchAccessory.ButtonCount; i++)
  {
    button[i] = false;
  }
  
  activated = true;
}

void StatelessSwitch::UPDATE(boolean force)
{
  for(int i = 0; i < statelessswitchAccessory.ButtonCount; i++)
  {
    boolean buttontmp = digitalRead(statelessswitchAccessory.ButtonControlPositive[i]);
    
    if(buttontmp && !button[i] && !statelessswitchAccessory.ButtonLockPositive[i])
    {
      button[i] = true;
      statelessswitchAccessory.ButtonLockPositive[i] = true;
    }
    else if(buttontmp && button[i] && !statelessswitchAccessory.ButtonLockPositive[i])
    {
      button[i] = false;
      statelessswitchAccessory.ButtonLockPositive[i] = true;
    }
    
    if(!buttontmp && statelessswitchAccessory.ButtonLockPositive[i])
    {
      if(!buttontmp)
      {
        statelessswitchAccessory.ButtonLockPositive[i] = false;
      }
  
      Serial.println("Schalter Nr. " + String(i + 1) + ": " + (button[i] ? "An" : "Aus"));
  
      String response = statelessswitchAccessory.safeFetch(statelessswitchAccessory.BridgeIP + ":" + statelessswitchAccessory.WebhookPort + "/devices?id=" + WiFi.macAddress() + "&event=" + i + "&value=" + (button[i] ? 0 : 1), 10000, false)[0];
  
      if(response == "OK" && statelessswitchAccessory.LED)
      {
        if(statelessswitchAccessory.ButtonCount == 1)
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
