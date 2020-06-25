#include "climate.h"
#include "accessory.h"
#include "Arduino.h"
#include <DHT.h>

Accessory main;
DHT dht(2, DHT11);

unsigned long previousMillis;

float temp;
float hum;

Climate::Climate() { }

void Climate::SETUP(String ip, String port, int interval, String events)
{
  main.SETUP("climate", "5.0.0", interval, events, ip, port);

  dht.begin();
}

void Climate::UPDATE(boolean force)
{
  unsigned long currentMillis = millis();

  if(force || currentMillis - previousMillis >= main.Interval)
  {
    previousMillis = currentMillis;   

    float humtmp = dht.readHumidity();
    float temptmp = dht.readTemperature();

    if(isnan(humtmp) || isnan(temptmp))
    {
      Serial.println("Fehler beim Lesen des DHT Sensors!");
    }
    else
    {
      Serial.print("Temperatur: " + String(temptmp) + " - Feuchtigkeit: " + String((int)humtmp) + "%");
      
      Serial.println(" ( " + String(temp - temptmp >= 0.5 || temp - temptmp <= -0.5 ? "X" : "O") + " | " + String(humtmp != hum ? "X" : "O") + " )");
      
      if(temp - temptmp >= 0.5 || temp - temptmp <= -0.5)
      {
        temp = temptmp;

        main.safeFetch(main.BridgeIP + ":" + String(main.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&type=temperature&value=" + String(temp), main.Interval, false);

        for(int i = 0; i < main.EventsNegative; i++)
        {
          if(temp < main.EventControlNegative[i] && !main.EventLockNegative[i])
          {
            int response = main.safeFetch(main.BridgeIP + ":" + String(main.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&event=" + i, main.Interval, false);

            if(response == HTTP_CODE_OK)
            {
              for(int j = 0; j < main.EventsPositive; j++)
              {
                main.EventLockPositive[j] = false;
              }
              
              main.EventLockNegative[i] = true;
      
              Serial.println("( " + String(i) + " ) Scene wird aktiviert!");
            }
            else
            {
              Serial.println("( " + String(i) + " ) Scene konnte nicht aktiviert werden!");
            }
          }
          
          for(int i = 0; i < main.EventsPositive; i++)
          {  
            if(temp > main.EventControlPositive[i] && !main.EventLockPositive[i])
            {
              int response = main.safeFetch(main.BridgeIP + ":" + String(main.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&event=" + String(i + main.EventsNegative), main.Interval, false);

              if(response == HTTP_CODE_OK)
              {
                for(int j = 0; j < main.EventsNegative; j++)
                {
                  main.EventLockNegative[j] = false;
                }
                
                main.EventLockPositive[i] = true;
                
                Serial.println("( " + String(i + main.EventsNegative) + " ) Scene wird aktiviert!");
              }
              else
              {
                Serial.println("( " + String(i + main.EventsNegative) + " ) Scene konnte nicht aktiviert werden!");
              }
            }
          }
        } 
      }
      
      if(humtmp != hum)
      {
        hum = humtmp;

        main.safeFetch(main.BridgeIP + ":" + String(main.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&type=humidity&value=" + String((int)hum), main.Interval, false);
      }
    }
  }
}
