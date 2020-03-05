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
    //getSwitch();
  }
}

boolean button;
/*
void getSwitch()
{
  boolean contacttmp = digitalRead(14);

  if(contacttmp != contact)
  {
    contact = contacttmp;

    if(contact)
    {
      m.sender.begin("http://syntex.local:1710/devices?mac=" + WiFi.macAddress() + "&type=" + m.Type + "&value=false");
      Serial.println("Kontakt: Nein");
    }
    else
    {
      m.sender.begin("http://syntex.local:1710/devices?mac=" + WiFi.macAddress() + "&type=" + m.Type + "&value=true");
      Serial.println("Kontakt: Ja");
    }
    
    m.sender.GET();
    m.sender.end();
  }
}
*/
