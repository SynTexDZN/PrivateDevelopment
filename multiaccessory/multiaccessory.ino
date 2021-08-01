#include <ArduinoJson.h>
#include "main.h"
#include "climate.h"
#include "temperature.h"
#include "light.h"
#include "rain.h"
#include "motion.h"
#include "contact.h"
#include "relais.h"
#include "statelessswitch.h"
#include "lcd.h"
#include "status_led.h"
#include "buzzer.h"

SynTexMain m;
Climate climate(2);
Temperature temperature(2);
Light light;
Rain rain(16);
Motion motion(14);
Contact contact(5);
Relais relais(0);
StatelessSwitch button(5);
LCD lDisplay;
StatusLED sLED;
Buzzer buzzer(13);

void setup()
{
  sLED.setupRGB();
  
  if(m.SETUP("6.3.2", "[]", "[]", "") && m.checkConnection())
  {
    if(hasConfig("status-led"))
    {
      sLED.SETUP(m.LED, 2);
      
      m.LED = false;
    }

    sLED.finishSetupRGB();

    if(hasConfig("hall-sensor"))
    {
      contact.isHallSensor = true;
    }

    if(hasConfig("dht22"))
    {
      climate.setDHT22();
    }

    if(hasService("temperature") && hasService("humidity"))
    {
      climate.SETUP(m.BridgeIP, m.WebhookPort, m.Interval, m.LED);
    }
    else if(hasService("temperature"))
    {
      temperature.SETUP(m.BridgeIP, m.WebhookPort, m.Interval, m.LED);
    }

    if(hasService("light"))
    {
      light.SETUP(m.BridgeIP, m.WebhookPort, m.Interval, m.LED);
    }

    if(hasService("rain"))
    {
      rain.SETUP(m.BridgeIP, m.WebhookPort, m.Interval, m.LED);
    }
    
    if(hasService("motion"))
    {
      motion.SETUP(m.BridgeIP, m.WebhookPort, m.Interval, m.LED);
    }

    if(hasService("contact"))
    {
      contact.SETUP(m.BridgeIP, m.WebhookPort, m.Interval, m.LED);
    }

    if(hasService("relais"))
    {
      relais.SETUP(m.BridgeIP, m.WebhookPort, m.LED, m.server);
    }

    if(hasService("button"))
    {
      button.SETUP(m.BridgeIP, m.WebhookPort, m.Buttons, m.LED, m.server);
    }

    if(hasService("lcd"))
    {
      lDisplay.SETUP(m.BridgeIP, m.WebhookPort, m.Interval, m.LED, m.Name, m.Version, m.server);
    }

    if(hasService("buzzer"))
    {
      buzzer.SETUP(m.BridgeIP, m.WebhookPort, m.LED, m.server);
    }

    if(m.Active)
    {
      if(climate.activated)
      {
        climate.UPDATE(true);
      }

      if(temperature.activated)
      {
        temperature.UPDATE(true);
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

    if(temperature.activated)
    {
      temperature.UPDATE(false);
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
      if(climate.activated && light.activated)
      {
        String info[] = {"Klima: " + String(climate.tempExact).substring(0, String(climate.tempExact).length() - 1) + " \337C - " + String((int)climate.humExact) + " %", "Licht: " + String((int)light.light) + " Lux"};
        lDisplay.UPDATE(2, info);
      }
      else if(climate.activated)
      {
        String info[] = {"Klima: " + String(climate.tempExact).substring(0, String(climate.tempExact).length() - 1) + " \337C - " + String((int)climate.humExact) + " %"};
        lDisplay.UPDATE(1, info);
      }
      else if(hasConfig("doorbell"))
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

boolean hasConfig(String key)
{
  StaticJsonDocument<400> doc;
  deserializeJson(doc, m.Suffix);

  for(JsonVariant k : doc.as<JsonArray>())
  {
    if(k.as<String>() == key)
    {
      return true;
    }
  }
  
  return false;
}

boolean hasService(String key)
{
  StaticJsonDocument<400> doc;
  deserializeJson(doc, m.Services);

  for(JsonVariant k : doc.as<JsonArray>())
  {
    if(k.as<String>() == key)
    {
      return true;
    }
  }
  
  return false;
}
