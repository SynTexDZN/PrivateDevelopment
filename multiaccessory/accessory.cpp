#include "main.h"
#include "accessory.h"
#include "Arduino.h"
#include "ArduinoJson.h"

Accessory::Accessory() {}

boolean Accessory::SETUP(String Version, int Interval, String Buttons, String BridgeIP, String WebhookPort, boolean LED)
{
  this -> BridgeIP = BridgeIP;
  this -> WebhookPort = WebhookPort;
  this -> Interval = Interval;
  this -> Version = Version;
  this -> LED = LED;
  
  StaticJsonDocument<400> doc;
  deserializeJson(doc, Buttons);
  JsonArray buttons = doc.as<JsonArray>();
  
  ButtonCount = 0; 
  
  for(JsonVariant v : buttons)
  {
    ButtonCount++;
  }

  ButtonControlPositive = new int [ButtonCount];
  int c = 0;
  
  for(int j = 0; j < ButtonCount; j++)
  {
    if(buttons[j].as<int>() > 0)
    {
      ButtonControlPositive[c] = buttons[j].as<int>();
      c++;
    }
  }

  ButtonLockPositive = new boolean [ButtonCount];
  
  for(int i = 0; i < ButtonCount; i++)
  {
    ButtonLockPositive[i] = false;
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
