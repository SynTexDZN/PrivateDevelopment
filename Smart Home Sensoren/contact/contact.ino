#include "main.h"

SynTexMain m;

boolean contact;

void setup()
{
  if(m.SETUP("contact", "4.3.4", 0, "[]") && m.checkConnection() && m.Active)
  {
    contact = digitalRead(14);
    
    m.safeFetch(m.BridgeIP + ":" + String(m.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&value=" + (contact ? "true" : "false"), 10, false);
  }
}

void loop()
{
  m.LOOP();

  if(m.checkConnection() && m.Active)
  {
    getContact();
  }
}

void getContact()
{
  boolean contacttmp = digitalRead(14);

  if(contacttmp != contact)
  {
    contact = contacttmp;

    Serial.print("Kontakt: ");
    Serial.println(contact ? "Nein" : "Ja");

    int response = m.safeFetch(m.BridgeIP + ":" + String(m.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&value=" + (contact ? "true" : "false"), 10, false);

    if(response == HTTP_CODE_OK && m.LED)
    {
      digitalWrite(LED_BUILTIN, contact ? LOW : HIGH);
    }
  }
}
