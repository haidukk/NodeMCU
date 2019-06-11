/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example runs directly on NodeMCU.

  Note: This requires ESP8266 support package:
    https://github.com/esp8266/Arduino

  Please be sure to select the right NodeMCU module
  in the Tools -> Board menu!

  For advanced settings please follow ESP examples :
   - ESP8266_Standalone_Manual_IP.ino
   - ESP8266_Standalone_SmartConfig.ino
   - ESP8266_Standalone_SSL.ino

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
#define Led            D6


#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <Servo.h>
#include <BlynkSimpleEsp8266.h>
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, Led, NEO_GRB + NEO_KHZ800);
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "f40591b74c1d4ae184493d82e27d81d1";
#define MotionPin 13 
Servo servo;
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Kostik";
char pass[] = "Sofik_pro";
int r = 0;
int g = 0;
int b = 0; 
int motionstate = 0;
int minSecsBetweenEmails = 20; // 1 min
long lastSend = -minSecsBetweenEmails * 1000; // змінна для запобігання відправки декількох листів одночасно

void setup()
{
  
  pinMode(MotionPin, INPUT);
   servo.attach(0);
  servo.write(0);
  // Debug console
  Serial.begin(9600);
  strip.begin();
  strip.setBrightness(100);
  strip.show(); 
  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
}

BLYNK_WRITE(V6){
   r = param[0].asInt();
   g = param[1].asInt();
   b = param[2].asInt(); 
}
BLYNK_WRITE(V1){
    servo.write(param.asInt());
}
 //i = strip.Color(r,g,b);
void loop()
{
          long now = millis();
  if (digitalRead(MotionPin) == HIGH) // перевіряємо що на пін подається сигнал високого рівня, тобто детектовано рух
  {
  if (now > (lastSend + minSecsBetweenEmails * 1000)) //додаткова перевірка від надлишкового спрацювання
  {  Blynk.email("Motion", "Alarm motion detected");
    lastSend = now;
  }
  }
  
  colorWipe(strip.Color(r,g,b), 100);
  strip.show();

  Blynk.run();
}
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
