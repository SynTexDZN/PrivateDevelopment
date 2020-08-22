#include "main.h"
#include "accessory.h"
#include "Arduino.h"
#include "ArduinoJson.h"

Accessory::Accessory() {}

boolean Accessory::SETUP(String Version, int Interval, String Events, String BridgeIP, String WebhookPort, boolean LED)
{
  this -> BridgeIP = BridgeIP;
  this -> WebhookPort = WebhookPort;
  this -> Interval = Interval;
  this -> Version = Version;
  this -> LED = LED;
  
  StaticJsonDocument<400> doc;
  deserializeJson(doc, Events);
  JsonArray events = doc.as<JsonArray>();
  
  EventsPositive = 0; 
  
  for(JsonVariant v : events)
  {
    EventsPositive++;
  }

  EventControlPositive = new int [EventsPositive];
  int c = 0;
  
  for(int j = 0; j < EventsPositive; j++)
  {
    if(events[j].as<int>() > 0)
    {
      EventControlPositive[c] = events[j].as<int>();
      c++;
    }
  }

  EventLockPositive = new boolean [EventsPositive];
  
  for(int i = 0; i < EventsPositive; i++)
  {
    EventLockPositive[i] = false;
  }
  
  return true;
}

int Accessory::safeFetch(String URL, int Time, boolean Dots)
{
  int response;
  int counter = 0;
  unsigned long fetchMillis;
  
  do
  {
    unsigned long currentMillis = millis();
    
    if(currentMillis - fetchMillis >= 500)
    {
      fetchMillis = currentMillis;
      
      sender.begin(URL);
      response = sender.GET();

      if(response != HTTP_CODE_OK)
      {
        //delay(500);
        counter++;

        if(Dots)
        {
          Serial.print(".");
        }
      }
    }
  }
  while(response != HTTP_CODE_OK && counter < Time / 500);

  if(response != HTTP_CODE_OK)
  {
    Serial.println("Fetch Fehlgeschlagen!");
  }

  if(Dots)
  {
    Serial.println();
  }

  return response;
}
