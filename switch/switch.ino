#include "main.h"

SynTexMain m;

boolean button;
boolean lock;

void setup()
{
  m.SETUP("switch", "3.0.0", 0);

  m.sender.begin("http://syntex.local:1710/devices?mac=" + WiFi.macAddress() + "&type=" + m.Type + "&value=false");
  m.sender.GET();
  m.sender.end();

  m.server.on("/switch", []
  {
    if(m.server.hasArg("value"))
    {
      if(m.server.arg("value") == "true")
      {
        button = true;
      }
      else if(m.server.arg("value") == "false")
      {
        button = false;
      }
    }
    else
    {
      boolean state = digitalRead(5);
      
      if(state)
      {
        button = false;
      }
      else
      {
        button = true;
      }
    }

    if(button)
    {
      Serial.println("Schalter: An");

      if(m.LED)
      {
        digitalWrite(LED_BUILTIN, LOW);
      }
    }
    else
    {
      Serial.println("Schalter: Aus");

      if(m.LED)
      {
        digitalWrite(LED_BUILTIN, HIGH);
      }
    }

    m.server.sendHeader("Access-Control-Allow-Origin", "*");
    m.server.send(200, "text/plain", String(digitalRead(5)));
  });
}

void loop()
{
  m.LOOP();

  if(m.checkConnection())
  {
    getSwitch();
  }
}

void getSwitch()
{
  boolean buttontmp = digitalRead(5);

  if(buttontmp && !button && !lock)
  {
    button = true;
    lock = true;
  }
  else if(buttontmp && button && !lock)
  {
    button = false;
    lock = true;
  }

  if(!buttontmp && lock)
  {
    if(button)
    {
      m.sender.begin("http://syntex.local:1710/devices?mac=" + WiFi.macAddress() + "&type=" + m.Type + "&value=true");

      if(m.LED)
      {
        digitalWrite(LED_BUILTIN, LOW);
      }
      
      Serial.println("Schalter: An");
    }
    else
    {
      m.sender.begin("http://syntex.local:1710/devices?mac=" + WiFi.macAddress() + "&type=" + m.Type + "&value=false");

      if(m.LED)
      {
        digitalWrite(LED_BUILTIN, HIGH);
      }
      
      Serial.println("Schalter: Aus");
    }
    
    m.sender.GET();
    m.sender.end();
  }

  if(!buttontmp)
  {
    lock = false;
  }
}
