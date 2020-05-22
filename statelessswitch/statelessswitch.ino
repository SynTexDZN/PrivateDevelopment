#include "main.h"

SynTexMain m;

boolean *button;

void setup()
{  
  if(m.SETUP("statelessswitch", "4.3.3", 0, "[5]") && m.checkConnection())
  {    
    button = new boolean[m.EventsPositive];
    
    for(int i = 0; i < m.EventsPositive; i++)
    {
      button[i] = false;
    }

    delay(500);

    if(m.LED && m.EventsPositive == 1)
    {
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}

void loop()
{
  m.LOOP();

  if(m.checkConnection() && m.Active)
  {
    for(int i = 0; i < m.EventsPositive; i++)
    {
      getSwitch(i, m.EventControlPositive[i]);
    }
  }
}

void getSwitch(int i, int pin)
{
  boolean buttontmp = digitalRead(pin);
  
  if(buttontmp && !button[i] && !m.EventLockPositive[i])
  {
    button[i] = true;
    m.EventLockPositive[i] = true;
  }
  else if(buttontmp && button[i] && !m.EventLockPositive[i])
  {
    button[i] = false;
    m.EventLockPositive[i] = true;
  }
  
  if(!buttontmp && m.EventLockPositive[i])
  {
    if(!buttontmp)
    {
      m.EventLockPositive[i] = false;
    }

    Serial.println("Schalter Nr. " + String(i + 1) + ": " + (button[i] ? "An" : "Aus"));

    int response = m.safeFetch(m.BridgeIP + ":" + m.WebhookPort + "/devices?mac=" + WiFi.macAddress() + "&event=" + i + "&value=" + (button[i] ? 0 : 1), 10, false);

    if(response == HTTP_CODE_OK && m.LED)
    {
      if(m.EventsPositive == 1)
      {
        digitalWrite(LED_BUILTIN, button[i] ? HIGH : LOW);
      }
      else
      {
        digitalWrite(LED_BUILTIN, LOW);
        delay(button[i] ? 1000 : 250);
        digitalWrite(LED_BUILTIN, HIGH);
      }
    }
  }
}
