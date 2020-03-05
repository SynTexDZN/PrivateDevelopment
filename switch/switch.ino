#include "main.h"

SynTexMain m;

void setup()
{
  m.SETUP("switch", "3.0.0", 0);

  m.sender.begin("http://syntex.local:1710/devices?mac=" + WiFi.macAddress() + "&type=" + m.Type + "&value=false");
  m.sender.GET();
  m.sender.end();

  pinMode(5, OUTPUT);

  m.server.on("/switch", []
  {
    if(m.server.hasArg("value"))
    {
      if(m.server.arg("value") == "true")
      {
        digitalWrite(5, HIGH);
      }
      else if(m.server.arg("value") == "false")
      {
        digitalWrite(5, LOW);
      }
    }
    else
    {
      boolean state = digitalRead(5);
      
      if(state)
      {
        digitalWrite(5, LOW);
      }
      else
      {
        digitalWrite(5, HIGH);
      }
    }

    if(digitalRead(5))
    {
      Serial.println("Relais: An");
    }
    else
    {
      Serial.println("Relais: Aus");
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

boolean button;
boolean lock;

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
      Serial.println("Schalter: An");
    }
    else
    {
      m.sender.begin("http://syntex.local:1710/devices?mac=" + WiFi.macAddress() + "&type=" + m.Type + "&value=false");
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
