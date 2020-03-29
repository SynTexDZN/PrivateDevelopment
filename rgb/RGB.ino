#include "main.h"

SynTexMain m;

int r;
int g;
int b; 
boolean state;

void setup()
{
  if(m.SETUP("rgb", "3.7.0", 0) && m.checkConnection())
  {
    pinMode(5, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(0, OUTPUT);
  
    m.sender.begin(m.BridgeIP + ":" + String(m.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&value=false:0:0:0");
    m.sender.GET();
    m.sender.end();
  
    m.server.on("/color", []
    {
      if(m.server.hasArg("r") && m.server.hasArg("g") && m.server.hasArg("b"))
      {
          setRGB(m.server.arg("r").toInt(), m.server.arg("g").toInt(), m.server.arg("b").toInt());
          
          m.server.sendHeader("Access-Control-Allow-Origin", "*");
          m.server.send(200, "text/plain", "Success");
      }
    });
  }
}

void loop()
{
  m.LOOP();
}

void setRGB(int red, int green, int blue)
{
  if(red == 0 && green == 0 && blue == 0)
  {
    state = false;

    Serial.println(String(red) + " + " + String(green) + " + " + String(blue));
    
    analogWrite(5, 0);
    analogWrite(4, 0);
    analogWrite(0, 0);
  }
  else
  {
    state = true;
    r = red;
    g = green;
    b = blue;

    Serial.println(String(r) + " + " + String(g) + " + " + String(b));
    
    analogWrite(5, r * 4);
    analogWrite(4, g * 4);
    analogWrite(0, b * 4);
  }
  
  m.sender.begin(m.BridgeIP + ":" + String(m.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&value=" + (state ? "true" : "false") + ":" + String(r) + ":" + String(g) + ":" + String(b));
  m.sender.GET();
  m.sender.end();
}
