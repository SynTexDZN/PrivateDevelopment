#include "main.h"

SynTexMain m;

int r;
int g;
int b; 
boolean state;

void setup()
{
  if(m.SETUP("rgb", "4.3.2", 0, "[]") && m.checkConnection())
  {
    pinMode(5, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(0, OUTPUT);

    m.server.on("/color", []
    {
      if(m.server.hasArg("rgb"))
      {
          setRGB(split(m.server.arg("rgb"), ',', 0).toInt(), split(m.server.arg("rgb"), ',', 1).toInt(), split(m.server.arg("rgb"), ',', 2).toInt());

          m.server.sendHeader("Access-Control-Allow-Origin", "*");
          m.server.send(200, "text/plain", "Success");
      }
    });

    m.safeFetch(m.BridgeIP + ":" + String(m.WebhookPort) + "/devices?mac=" + WiFi.macAddress() + "&value=false:0:0:0", 10, false);
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
}

String split(String s, char parser, int index)
{
  String rs = "";
  int parserIndex = index;
  int parserCnt = 0;
  int rFromIndex = 0, rToIndex = -1;
  
  while(index >= parserCnt)
  {
    rFromIndex = rToIndex + 1;
    rToIndex = s.indexOf(parser,rFromIndex);
    
    if(index == parserCnt)
    {
      if (rToIndex == 0 || rToIndex == -1) return "";
      return s.substring(rFromIndex, rToIndex);
    } else parserCnt++;
  }
  return rs;
}