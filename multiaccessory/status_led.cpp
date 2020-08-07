#include "status_led.h"
#include "Arduino.h"

int r = 12;
int g = 15;
int b = 13;

int color[] = { 0, 30, 255 };
int next[] = { 0, 0, 0 };
int tmp[] = { 0, 0, 0 };

StatusLED::StatusLED() {}

void StatusLED::SETUP(boolean LED, int Fade)
{
  this -> LED = LED;
  this -> Fade = Fade;

  //flashRGB(0, 255, 0, 1000);
  
  activated = true;
}

void StatusLED::setupRGB()
{
  pinMode(r, OUTPUT);
  pinMode(g, OUTPUT);
  pinMode(b, OUTPUT);
  
  analogWrite(r, 0);
  analogWrite(g, 30);
  analogWrite(b, 255);
}

boolean offline;

void StatusLED::UPDATE()
{
  if(WiFi.status() != WL_CONNECTED && !offline)
  {
    blinkMillis = millis();
    Blink = 1000;
    Flash = 0;

    next[0] = 255;
    next[1] = 0;
    next[2] = 0;
  
    tmp[0] = 255;
    tmp[1] = 0;
    tmp[2] = 0;

    offline = true;
  }
  else if(!LED)
  {
    Blink = 0;
    Flash = 0;
    next[0] = 0;
    next[1] = 0;
    next[2] = 0;
  }

  if(WiFi.status() == WL_CONNECTED)
  {
    offline = false;
  }

  if(next[0] != color[0] || next[1] != color[1] || next[2] != color[2])
  {
    if(Fade > 0)
    {
      unsigned long currentMicros = micros();

      if(currentMicros - fadeMicros >= Fade * 250)
      {
        fadeMicros = currentMicros;
  
        if(next[0] != color[0])
        {
          if(next[0] > color[0])
          {
            color[0] += 1;
          }
          else
          {
            color[0] -= 1;
          }
        }
      
        if(next[1] != color[1])
        {
          if(next[1] > color[1])
          {
            color[1] += 1;
          }
          else
          {
            color[1] -= 1;
          }
        }
      
        if(next[2] != color[2])
        {
          if(next[2] > color[2])
          {
            color[2] += 1;
          }
          else
          {
            color[2] -= 1;
          }
        }
  
        analogWrite(r, color[0]);
        analogWrite(g, color[1]);
        analogWrite(b, color[2]);
      }
    }
    else
    {
      color[0] = next[0];
      color[1] = next[1];
      color[2] = next[2];
      
      analogWrite(r, color[0]);
      analogWrite(g, color[1]);
      analogWrite(b, color[2]);
    }
  }

  if(Flash > 0)
  {
    unsigned long currentMillis = millis();

    if(currentMillis - flashMillis >= Flash)
    {
      next[0] = 0;
      next[1] = 0;
      next[2] = 0;

      Flash = 0;
    }
  }

  if(Blink > 0)
  {
    unsigned long currentMillis = millis();

    if(currentMillis - blinkMillis >= Blink)
    {
      next[0] = tmp[0];
      next[1] = tmp[1];
      next[2] = tmp[2];
    }
    
    if(currentMillis - blinkMillis >= Blink * 2)
    {
      next[0] = 0;
      next[1] = 0;
      next[2] = 0;

      blinkMillis = currentMillis;
    }
  }
}

boolean cancel;

void StatusLED::setRGB(int red, int green, int blue)
{
  Blink = 0;
  Flash = 0;

  next[0] = red;
  next[1] = green;
  next[2] = blue;
}

void StatusLED::flashRGB(int red, int green, int blue, int t)
{
  flashMillis = millis();
  Flash = t;
  Blink = 0;

  next[0] = red;
  next[1] = green;
  next[2] = blue;
}

void StatusLED::blinkRGB(int red, int green, int blue, int s)
{
  blinkMillis = millis();
  Blink = s;
  Flash = 0;

  next[0] = red;
  next[1] = green;
  next[2] = blue;

  tmp[0] = red;
  tmp[1] = green;
  tmp[2] = blue;
}
