#include <ArduinoJson.h>
#include "main.h"
#include "climate.h"
#include "light.h"
#include "rain.h"
#include "motion.h"
#include "contact.h"
#include "relais.h"
#include "statelessswitch.h"
#include "lcd.h"
#include "status_led.h"

SynTexMain m;
Climate climate(2);
Light light;
Rain rain(16);
Motion motion(14);
Contact contact(5);
Relais relais(4);
StatelessSwitch button(5);
LCD lDisplay;
StatusLED sLED;

void setup()
{
  sLED.setupRGB();
  
  if(m.SETUP("6.0.4", "[]", "[]", "") && m.checkConnection())
  {
    if(m.Suffix == "status-led")
    {
      sLED.SETUP(m.LED, 2);
      
      m.LED = false;
    }

    sLED.finishSetupRGB();
    
    StaticJsonDocument<400> doc;
    deserializeJson(doc, m.Services);

    for(JsonVariant v : doc.as<JsonArray>())
    {
      if((v.as<String>() == "temperature" || v.as<String>() == "humidity") && !climate.activated)
      {
        climate.SETUP(m.BridgeIP, m.WebhookPort, m.Interval, m.LED);
      }

      if(v.as<String>() == "light")
      {
        light.SETUP(m.BridgeIP, m.WebhookPort, m.Interval, m.LED);
      }

      if(v.as<String>() == "rain")
      {
        rain.SETUP(m.BridgeIP, m.WebhookPort, m.Interval, m.LED);
      }
      
      if(v.as<String>() == "motion")
      {
        motion.SETUP(m.BridgeIP, m.WebhookPort, m.Interval, m.LED);
      }

      if(v.as<String>() == "contact")
      {
        contact.SETUP(m.BridgeIP, m.WebhookPort, m.Interval, m.LED);
      }

      if(v.as<String>() == "relais")
      {
        relais.SETUP(m.BridgeIP, m.WebhookPort, m.LED, m.server);
      }

      if(v.as<String>() == "button")
      {
        button.SETUP(m.BridgeIP, m.WebhookPort, m.Buttons, m.LED, m.server);
      }

      if(v.as<String>() == "lcd")
      {
        lDisplay.SETUP(m.BridgeIP, m.WebhookPort, m.Interval, m.LED, m.Name, m.Version, m.server);
      }
    }

    if(m.Active)
    {
      if(climate.activated)
      {
        climate.UPDATE(true);
      }

      if(light.activated)
      {
        light.UPDATE(true);
      }

      if(rain.activated)
      {
        rain.UPDATE(true);
      }
      
      if(motion.activated)
      {
        motion.UPDATE(true);
      }

      if(contact.activated)
      {
        contact.UPDATE(true);
      }

      if(button.activated)
      {
        button.UPDATE(true);
      }
    }
  }
}

boolean temp;

void loop()
{
  m.LOOP();

  if(m.checkConnection() && m.Active)
  {
    if(climate.activated)
    {
      climate.UPDATE(false);
    }

    if(light.activated)
    {
      light.UPDATE(false);
    }

    if(rain.activated)
    {
      rain.UPDATE(false);
    }
    
    if(motion.activated)
    {
      motion.UPDATE(false);
    }

    if(contact.activated)
    {
      contact.UPDATE(false);
    }

    if(button.activated)
    {
      button.UPDATE(false);
    }

    if(lDisplay.activated)
    {
      if(m.Suffix == "base")
      {
        String info[] = {"Klima: " + String(climate.temp).substring(0, String(climate.temp).length() - 1) + " \337C - " + String((int)climate.hum) + " %", "Licht: " + String((int)light.light) + " Lux"};
        lDisplay.UPDATE(2, info);
      }
      else if(m.Suffix == "doorbell")
      {
        String info[] = {"Bitte: Klingeln!"};
        lDisplay.UPDATE(1, info);
      }
    }

    boolean device = motion.motion;

    if(sLED.activated && temp != device)
    {
      temp = device;

      if(device)
      {
        sLED.setRGB(255, 30, 0);
      }
      else
      {
        sLED.setRGB(0, 0, 0);
      }
    }
  }

  if(sLED.activated)
  {
    sLED.UPDATE();
  }
}
