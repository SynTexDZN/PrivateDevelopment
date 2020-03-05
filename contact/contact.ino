#include "main.h"

SynTexMain m;

void setup()
{
  m.SETUP("contact", "3.1.1", 0);

  m.sender.begin("http://syntex.local:1710/devices?mac=" + WiFi.macAddress() + "&value=true");
  m.sender.GET();
  m.sender.end();
}

void loop()
{
  m.LOOP();

  if(m.checkConnection())
  {
    getContact();
  }
}

boolean contact;

void getContact()
{
  boolean contacttmp = digitalRead(14);

  if(contacttmp != contact)
  {
    contact = contacttmp;

    if(contact)
    {
      m.sender.begin("http://syntex.local:1710/devices?mac=" + WiFi.macAddress() + "&value=false");

      if(m.LED)
      {
        digitalWrite(LED_BUILTIN, LOW);
      }
      
      Serial.println("Kontakt: Nein");
    }
    else
    {
      m.sender.begin("http://syntex.local:1710/devices?mac=" + WiFi.macAddress() + "&value=true");

      if(m.LED)
      {
        digitalWrite(LED_BUILTIN, HIGH);
      }
      
      Serial.println("Kontakt: Ja");
    }
    
    m.sender.GET();
    m.sender.end();
  }
}
