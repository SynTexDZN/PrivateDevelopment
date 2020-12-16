#include "contact.h"
#include "accessory.h"
#include "Arduino.h"

Accessory contactAccessory;

Contact::Contact(int Pin)
{
  this -> Pin = Pin;
}

void Contact::SETUP(String ip, String port, int interval, boolean led)
{
  contactAccessory.SETUP("1.0.1", interval, "[]", ip, port, led);
  
  activated = true;
}

void Contact::UPDATE(boolean force)
{
  unsigned long currentMillis = millis();

  boolean contacttmp = digitalRead(Pin);

  if(force || contacttmp != contact)
  {
    contact = contacttmp;

    Serial.print("Kontakt: ");
    Serial.println(contact ? "Nein" : "Ja");

    int response = contactAccessory.safeFetch(contactAccessory.BridgeIP + ":" + String(contactAccessory.WebhookPort) + "/devices?id=" + WiFi.macAddress() + "&type=contact&value=" + (contact ? "true" : "false"), 10000, false);

    if(response == HTTP_CODE_OK && contactAccessory.LED)
    {
      digitalWrite(LED_BUILTIN, contact ? LOW : HIGH);
    }
  }
}
