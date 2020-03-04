#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

HTTPClient sender;

void setup()
{
  Serial.begin(115200);
  Serial.println("ESP Gestartet");
  WiFi.begin("WLAN-C6F2BC V4", "9182113706784115");
  Serial.print("Verbindung wird hergestellt ..");
  
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Verbunden! IP-Adresse: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  getAirQuality();
}

float quality;
unsigned long previousMillis = -500;
long interval = 500;

void getAirQuality()
{
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;   
 
    float qualitytmp = 1.0 + analogRead(A0) / 256.0;

    float t1;
    float t2;
    float t3;

    t1=(analogRead(A0)/1024.0) * 3300;
    Serial.print("smoke: ");
    Serial.println(t1);

    t2=(t1/10);
    Serial.print("celc: ");
    Serial.println(t2);

    t3=((t2 * 9)/5 + 32);
    Serial.print("fhrn: ");
    Serial.println(t3);
    
    if(qualitytmp != quality)
    {
      quality = qualitytmp;
      sender.begin("http://192.168.188.121:51828/?accessoryId=sensor7&value=" + String(quality));
      sender.GET();
      sender.end();
    }

    Serial.println("Luftqualität: " + String(quality));
  }
}
