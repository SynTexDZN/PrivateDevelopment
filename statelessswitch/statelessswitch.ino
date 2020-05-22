#include "main.h"

SynTexMain m;

boolean *button;

void setup()
{  
  if(m.SETUP("statelessswitch", "4.2.0", 0, "[5]") && m.checkConnection())
  {    
    button = new boolean[m.EventsPositive];
    
    for(int i = 0; i < m.EventsPositive; i++)
    {
      button[i] = false;
      
      if(m.Active)
      {
        getSwitch(i, m.EventControlPositive[i]);
      }
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
    m.sender.begin(m.BridgeIP + ":" + m.WebhookPort + "/devices?mac=" + WiFi.macAddress() + "&event=" + i + "&value=" + (button[i] ? 0 : 1));
    m.sender.GET();
    m.sender.end();

    Serial.println("Schalter Nr. " + String(i+1) + ": " + (button[i] ? "An" : "Aus"));

    if(m.LED)
    {
      if(m.EventsPositive == 1)
      {
        digitalWrite(LED_BUILTIN, button[i] ? LOW : HIGH);
      }
      else
      {
        digitalWrite(LED_BUILTIN, LOW);
        delay(button[i] ? 1000 : 250);
        digitalWrite(LED_BUILTIN, HIGH);
      }
    }
  }

  if(!buttontmp)
  {
    m.EventLockPositive[i] = false;
  }
}
