#include "buzzer.h"
#include "accessory.h"
#include "Arduino.h"

Accessory buzzerAccessory;

Buzzer::Buzzer(int Pin)
{
  this -> Pin = Pin;
}

void Buzzer::SETUP(String ip, String port, boolean led, ESP8266WebServer &server)
{
  buzzerAccessory.SETUP("1.0.0", 10000, "[]", ip, port, led);

  pinMode(Pin, OUTPUT);
  
  server.on("/buzzer", [&] ()
  {
    int frequency = 1920;

    if(server.hasArg("frequency"))
    {
      frequency = server.arg("frequency").toInt();
    }
    
    if(server.hasArg("value"))
    {
      if(server.arg("value") == "true")
      {
        tone(Pin, frequency);
      }
      else
      {
        noTone(Pin);
      }
    }

    if(server.hasArg("preset"))
    {
      if(server.arg("preset") == "doorbell1")
      {
        for(int i = 0; i < 3; i++)
        {
          tone(Pin, frequency);

          delay(500);

          noTone(Pin);

          delay(1000);
        }
      }

      if(server.arg("preset") == "doorbell2")
      {
        for(int i = 0; i < 3; i++)
        {
          tone(Pin, frequency);

          delay(100);

          noTone(Pin);

          delay(1000);
        }
      }

      if(server.arg("preset") == "doorbell3")
      {
        for(int i = 0; i < 3; i++)
        {
          tone(Pin, frequency);

          delay(100);

          noTone(Pin);

          delay(100);
        }

        delay(500);

        for(int i = 0; i < 3; i++)
        {
          tone(Pin, frequency);

          delay(100);

          noTone(Pin);

          delay(100);
        }
      }
    }

    boolean buzzer = digitalRead(Pin);

    Serial.print("Buzzer: ");
    Serial.println(buzzer ? "An" : "Aus");

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", buzzer ? "true" : "false");

    if(buzzerAccessory.LED)
    {
      digitalWrite(LED_BUILTIN, buzzer ? LOW : HIGH);
    }
  });

  activated = true;
}
