#include "accessory.h"
#include "Arduino.h"

Accessory::Accessory()
{
  Serial.println("New object created!");
}

//String SetupEvents;

boolean Accessory::SETUP(String Type, String Version, int Interval, String Events, String BridgeIP, String WebhookPort)
{
  this -> BridgeIP = BridgeIP;
  this -> WebhookPort = WebhookPort;
  this -> Interval = Interval;
  this -> Type = Type;
  this -> Version = Version;
  
  //SetupEvents = Events;
  
  EventsNegative = 0; 
  EventsPositive = 0; 
/*
  JsonArray events = obj["events"].as<JsonArray>();

  for(JsonVariant v : events)
  {
    if(v.as<int>() < 0)
    {
      EventControlNegative[EventsNegative] = -v.as<int>();
      Serial.print("< " + String(EventControlNegative[EventsNegative]) + "  ");
      EventsNegative++;
    }
    else
    {
      EventControlPositive[EventsPositive] = v.as<int>();
      Serial.print("> " + String(EventControlPositive[EventsPositive]) + "  ");
      EventsPositive++;
    }
  }

  EventLockNegative = new boolean [EventsNegative];
  
  for(int i = 0; i < EventsNegative; i++)
  {
    EventLockNegative[i] = false;
  }

  EventLockPositive = new boolean [EventsPositive];
  
  for(int i = 0; i < EventsPositive; i++)
  {
    EventLockPositive[i] = false;
  }

  if(EventsPositive + EventsNegative == 0)
  {
    Serial.print("/");
  }
  
  Serial.println("");
  Serial.println("-------------");
  */
  return true;
}

int Accessory::safeFetch(String URL, int Time, boolean Dots)
{
  int response;
  int counter = 0;
  
  do
  {
    sender.begin(URL);
    response = sender.GET();

    if(response != HTTP_CODE_OK)
    {
      delay(500);
      counter++;

      if(Dots)
      {
        Serial.print(".");
      }
    }
  }
  while(response != HTTP_CODE_OK && counter < Time * 2);

  if(Dots)
  {
    Serial.println();
  }

  return response;
}
