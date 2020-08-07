#include "contact.h"
#include "accessory.h"
#include "Arduino.h"

Accessory contactAccessory;

Contact::Contact() {}

void Contact::SETUP(String ip, String port, int interval, boolean led)
{
  contactAccessory.SETUP("contact", "1.0.1", interval, "[]", ip, port, led);
  
  activated = true;
}

void Contact::UPDATE(boolean force)
{
  unsigned long currentMillis = millis();

  boolean contacttmp = digitalRead(14);

  if(force || contacttmp != contact)
  {
    contact = contacttmp;

    Serial.print("Kontakt: ");
    Serial.println(contact ? "Nein" : "Ja");

    int response = contactAccessory.safeFetch(contactAccessory.BridgeIP + ":" + String(contactAccessory.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&type=contact&value=" + (contact ? "true" : "false"), 10000, false);

    if(response == HTTP_CODE_OK && contactAccessory.LED)
    {
      digitalWrite(LED_BUILTIN, contact ? LOW : HIGH);
    }
  }
}
