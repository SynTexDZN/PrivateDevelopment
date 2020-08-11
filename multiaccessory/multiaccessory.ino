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
Climate climate;
Light light;
Rain rain;
Motion motion;
Contact contact;
Relais relais;
StatelessSwitch button;
LCD lDisplay;
StatusLED sLED;

void setup()
{
  /* Custom Status LED Part */

  //sLED.setupRGB();
  
  if(m.SETUP("motion", "6.0.0", 10000, "[]") && m.checkConnection())
  {
    //sLED.SETUP(m.LED, 2);

    if(sLED.activated)
    {
      m.LED = false;
    }
    
    //climate.SETUP(m.BridgeIP, m.WebhookPort, m.Interval, m.LED);
    light.SETUP(m.BridgeIP, m.WebhookPort, m.Interval, m.LED);
    //rain.SETUP(m.BridgeIP, m.WebhookPort, m.Interval, m.LED);
    motion.SETUP(m.BridgeIP, m.WebhookPort, m.Interval, m.LED);
    //contact.SETUP(m.BridgeIP, m.WebhookPort, m.Interval, m.LED);
    relais.SETUP(m.BridgeIP, m.WebhookPort, m.LED, m.server);
    //button.SETUP(m.BridgeIP, m.WebhookPort, m.Events, m.LED, m.server);
    //lDisplay.SETUP(m.BridgeIP, m.WebhookPort, m.Interval, m.LED, m.Name, m.Version);

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

      /* Custom LCD Display Part */

      //String info[] = {"Klima: " + String(climate.temp).substring(0, String(climate.temp).length() - 1) + " \337C - " + String((int)climate.hum) + " %", "Licht: " + String((int)light.light) + " Lux"};
      //lDisplay.UPDATE(2, info);
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

    /* Custom LCD Display Part */

    //String info[] = {"Klima: " + String(climate.temp).substring(0, String(climate.temp).length() - 1) + " \337C - " + String((int)climate.hum) + " %", "Licht: " + String((int)light.light) + " Lux"};
    //lDisplay.UPDATE(2, info);

    /* Custom Status LED Part */

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
