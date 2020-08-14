#include "lcd.h"
#include "accessory.h"
#include "Arduino.h"
#include "LiquidCrystal_I2C.h"
#include "RTClib.h"

LiquidCrystal_I2C lcd(0x27, 32, 2);
RTC_Millis rtc;
DateTime date;

Accessory displayAccessory;

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

LCD::LCD() {}

void LCD::SETUP(String IP, String Port, int Interval, boolean Backlight, String Name, String Version)
{
  this -> Interval = Interval;

  displayAccessory.SETUP("display", "1.0.0", Interval, "[]", IP, Port, Backlight);

  lcd.init();
  lcd.clear();

  byte ArrowUp[] = {
        B00100,
        B01110,
        B10101,
        B00100,
        B00100,
        B00100,
        B00100,
        B00000
      };

      byte ArrowDown[] = {
        B00100,
        B00100,
        B00100,
        B00100,
        B10101,
        B01110,
        B00100,
        B00000
      };

      byte Stable[] = {
        B01110,
        B00100,
        B00100,
        B00100,
        B00100,
        B00100,
        B01110,
        B00000
      };

      lcd.createChar(0, ArrowUp);
      lcd.createChar(1, ArrowDown);
      lcd.createChar(2, Stable);

  int response = displayAccessory.safeFetch(displayAccessory.BridgeIP + ":1711/serverside/time", 10000, false);
  
  if(response == HTTP_CODE_OK)
  {    
    DateTime ti = DateTime(displayAccessory.sender.getString().toInt());
    
    rtc.begin(ti);
  }

  if(Backlight)
  {
    lcd.backlight();
  }
  
  if(Name.length() < 16)
  {
    lcd.setCursor(0, 0);
    lcd.print(Name);
    
    lcd.setCursor(0, 1);
    lcd.print("Starten ..");  
  }
  else
  {
    String lineOne = split(Name, ' ', 0);
    String lineTwo = Name;

    lineTwo.replace(lineOne + " ", "");

    lcd.setCursor(0, 0);
    lcd.print(lineOne);
    
    lcd.setCursor(0, 1);
    lcd.print(lineTwo);
  }

  delay(1000);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Version");
  
  lcd.setCursor(0, 1); 
  lcd.print(Version);

  delay(1000);

  lcd.clear();

  //lcd.setCursor(0, 0);
  //lcd.print("Willkommen!");
  
  lcd.setCursor(0, 1); 
  lcd.print("Erfasse Daten ..");

  delay(1500);

  //lcd.clear();

  activated = true;
}

int counter = 0;

void LCD::UPDATE(int i, String* Infos)
{
  unsigned long currentMillis = millis();
  
  if(currentMillis - timeMillis >= 1000)
  {
    timeMillis = currentMillis;
    
    date = rtc.now();
    
    String realTime = "";

    if(date.hour() < 10)
    {
      realTime += "0";
    }

    realTime += String(date.hour()) + ":";

    if(date.minute() < 10)
    {
      realTime += "0";
    }
    
    realTime += String(date.minute()) + ":";
  
    if(date.second() < 10)
    {
      realTime += "0";
    }
    
    realTime += String(date.second());
    
    lcd.setCursor(8, 0);
    lcd.print(realTime);
  }

  if(currentMillis - previousMillis >= Interval / (i + 1))
  {
    previousMillis = currentMillis;

    lcd.setCursor(0, 0);
    lcd.print("        ");
    lcd.setCursor(0, 0);
    
    if(counter != i)
    {
      lcd.print(split(Infos[counter], ':', 0));
    }
    else
    {
      lcd.print("Datum");
    }

    String value = Infos[counter];

    value.replace(split(Infos[counter], ':', 0) + ": ", "");

    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);

    if(counter != i)
    {
      lcd.print(value);
    }
    else
    {
      String realDate = "";

      String weekDays[] = { "So", "Mo", "Di", "Mi", "Do", "Fr", "Sa" };
      realDate += weekDays[date.dayOfTheWeek()] + " ";

      if(date.day() < 10)
      {
        realDate += "0";
      }
  
      realDate += String(date.day()) + ".";
  
      if(date.month() < 10)
      {
        realDate += "0";
      }
      
      realDate += String(date.month()) + ".";
      realDate += String(date.year());
      
      lcd.print(realDate);
/*
      lcd.setCursor(0, 1);
      lcd.write((byte)0);
      lcd.print(" 27.1 \337C ");
      lcd.write((byte)2);
      lcd.print(" 34 %");
      */
    }

    if(counter < i)
    {
      counter++;
    }
    else
    {
      counter = 0;
    }
  }
}
