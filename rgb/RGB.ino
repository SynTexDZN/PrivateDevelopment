#include "main.h"

SynTexMain m;

int r;
int g;
int b; 
boolean state;

void setup()
{
  if(m.SETUP("rgb", "3.8.0", 0) && m.checkConnection())
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
  
    m.server.on("/switch", []
    {
      if(m.server.hasArg("value"))
      {
        state = (m.server.arg("value") == "true") ? true : false;
        
        if(state)
        {
          setRGB(r, g, b);
        }
        else
        {
          analogWrite(5, 0);
          analogWrite(4, 0);
          analogWrite(0, 0);
  
          String strState = state ? "true" : "false";
  
          m.sender.begin(m.BridgeIP + ":" + String(m.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&value=" + strState + ":" + String(r) + ":" + String(g) + ":" + String(b));
          m.sender.GET();
          m.sender.end();
        }
      }
      else
      {
        state ? state = false : state = true;
        
        String strState = state ? "true" : "false";
  
        Serial.println(strState);
  
        m.sender.begin(m.BridgeIP + ":" + String(m.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&value=" + strState + ":" + String(r) + ":" + String(g) + ":" + String(b));
        m.sender.GET();
        m.sender.end();
      }
  
      m.server.sendHeader("Access-Control-Allow-Origin", "*");
      m.server.send(200, "text/plain", "Success");
    });
  }
}

void loop()
{
  m.LOOP();
}

void setRGB(int red, int green, int blue)
{
  r = red;
  g = green;
  b = blue;

  Serial.println(String(r) + " + " + String(g) + " + " + String(b));
  
  analogWrite(5, r * 4);
  analogWrite(4, g * 4);
  analogWrite(0, b * 4);

  String strState = state ? "true" : "false";

  // TODO: Convert RGB to HSL in callback

  m.sender.begin(m.BridgeIP + ":" + String(m.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&value=" + strState + ":" + String(r) + ":" + String(g) + ":" + String(b));
  m.sender.GET();
  m.sender.end();
}
