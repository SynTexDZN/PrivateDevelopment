#include "main.h"
#include "accessory.h"
#include "Arduino.h"
#include "ArduinoJson.h"

Accessory::Accessory() {}

boolean Accessory::SETUP(String Type, String Version, int Interval, String Events, String BridgeIP, String WebhookPort, boolean LED)
{
  this -> BridgeIP = BridgeIP;
  this -> WebhookPort = WebhookPort;
  this -> Interval = Interval;
  this -> Type = Type;
  this -> Version = Version;
  this -> LED = LED;
  
  StaticJsonDocument<400> doc;
  deserializeJson(doc, Events);
  JsonArray events = doc.as<JsonArray>();
  
  EventsNegative = 0; 
  EventsPositive = 0; 
  
  for(JsonVariant v : events)
  {
    if(v.as<int>() < 0)
    {
      EventsNegative++;
    }
    else
    {
      EventsPositive++;
    }
  }

  EventControlPositive = new int [EventsPositive];
  int cp = 0;
  
  for(int j = 0; j < (EventsNegative + EventsPositive); j++)
  {
    if(events[j].as<int>() > 0)
    {
      EventControlPositive[cp] = events[j].as<int>();
      //Serial.println("# " + String(EventControlPositive[cp]) + "  " + String(cp));
      cp++;
    }
  }

  EventControlNegative = new int [EventsNegative];
  int cn = 0;
  
  for(int j = 0; j < EventsNegative + EventsPositive; j++)
  {
    if(events[j].as<int>() < 0)
    {
      EventControlNegative[cn] = -events[j].as<int>();
      //Serial.print("# " + String(EventControlNegative[cn]) + "  " + String(cn));
      cn++;
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
  
  return true;
}

void Accessory::updateScenes(int value)
{
  for(int i = 0; i < EventsNegative; i++)
  {
    //Serial.println(String(value) + " - " + String(EventControlNegative[i]) + " - " + String(EventLockNegative[i]) + String(i));
    
    if(value < EventControlNegative[i] && !EventLockNegative[i])
    {
      int response = safeFetch(BridgeIP + ":" + String(WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&event=" + i, Interval, false);

      if(response == HTTP_CODE_OK)
      {
        for(int j = 0; j < EventsPositive; j++)
        {
          EventLockPositive[j] = false;
        }
        
        EventLockNegative[i] = true;

        Serial.println("( " + String(i) + " ) Scene wird aktiviert!");
      }
      else
      {
        Serial.println("( " + String(i) + " ) Scene konnte nicht aktiviert werden!");
      }
    }
  }
    
  for(int i = 0; i < EventsPositive; i++)
  {  
    //Serial.println(String(value) + " - " + String(EventControlPositive[i]) + " - " + String(EventLockPositive[i]) + String(i));
    
    if(value > EventControlPositive[i] && !EventLockPositive[i])
    {
      int response = safeFetch(BridgeIP + ":" + String(WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&event=" + String(i + EventsNegative), Interval, false);

      if(response == HTTP_CODE_OK)
      {
        for(int j = 0; j < EventsNegative; j++)
        {
          EventLockNegative[j] = false;
        }
        
        EventLockPositive[i] = true;
        
        Serial.println("( " + String(i + EventsNegative) + " ) Scene wird aktiviert!");
      }
      else
      {
        Serial.println("( " + String(i + EventsNegative) + " ) Scene konnte nicht aktiviert werden!");
      }
    }
  }
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
