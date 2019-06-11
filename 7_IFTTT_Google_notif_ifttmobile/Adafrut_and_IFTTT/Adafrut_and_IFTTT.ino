 
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <Adafruit_NeoPixel.h>
#include <Servo.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <ArduinoOTA.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define Led            D6
#define BeepPin 14 
#define ServoPin 0       
#define MotionPin 13 


Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, Led, NEO_GRB + NEO_KHZ800);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

#define WLAN_SSID       "Kostik"             // Your SSID
#define WLAN_PASS       "Sofik_pro"        // Your password

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "GoogleEsp8266"            // Replace it with your username
#define AIO_KEY         "b7de1a07cbbb48a2a7bf3e2182f6d738"   // Replace with your Project Auth Key

/************ Global State (you don't need to change this!) ******************/
int motionstate = 0;
int minSecsBetweenEmails = 20; // 1 min
long lastSend = -minSecsBetweenEmails * 1000; // змінна для запобігання відправки декількох листів одночасно

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/


// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe Light1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/Led"); // FeedName
Adafruit_MQTT_Subscribe Screen = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/Screen"); // FeedName
Adafruit_MQTT_Subscribe slider = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/Servo");
Adafruit_MQTT_Subscribe Clear = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Clear");
Adafruit_MQTT_Subscribe Beep = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Beep");
Adafruit_MQTT_Subscribe color = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/color");
Adafruit_MQTT_Subscribe Motion = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Motion");
Adafruit_MQTT_Publish Move = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Move");
Servo servo;
  long hexColor = -1 ;
void MQTT_connect();

void setup() {
  Serial.begin(115200);

  servo.attach(ServoPin);
  servo.write(0);


    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  display.cp437(true);
  display.clearDisplay();
  
  pinMode(Led, OUTPUT);
  pinMode(BeepPin, OUTPUT);
  pinMode(MotionPin, INPUT);
  digitalWrite(BeepPin, HIGH);


  strip.begin();
  strip.setBrightness(50);
  strip.show(); 
  colorWipe(strip.Color(0, 0, 0), 50);
  
  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());
   ArduinoOTA.setHostname("Mqtt-server");
   ArduinoOTA.setPassword((const char *)"Haiduk");
   ArduinoOTA.begin();

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&Light1);
  mqtt.subscribe(&slider);
  mqtt.subscribe(&Screen);
  mqtt.subscribe(&Clear);
  mqtt.subscribe(&Beep);
  mqtt.subscribe(&color);
  mqtt.subscribe(&Motion);
   display.display();
  display.cp437(true);
  display.clearDisplay();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(10, 10);
  display.println(utf8rus("Гайдук\r"));
  display.display();
}

void loop() {
 
  MQTT_connect();
  ArduinoOTA.handle();
  Adafruit_MQTT_Publish *publishh;
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(200))) {
      if (subscription == &slider) {
      Serial.print(F("Slider: "));
      Serial.println((char *)slider.lastread);
      int sliderval = atoi((char *)slider.lastread);  // convert to a number
      servo.write(sliderval);
    }
     if (subscription == &Clear)
     {
      display.clearDisplay();
      display.display(); 
     }
     if (subscription == &Beep)
     {
     digitalWrite(BeepPin, LOW);
     delay(500);
     digitalWrite(BeepPin, HIGH);
      }
    if (subscription == &Light1) {
      Serial.print(F("Got: "));
      Serial.println((char *)Light1.lastread);
      int Light1_State = atoi((char *)Light1.lastread);
      if (Light1_State == 1)
      {
          colorWipe(strip.Color(148, 255, 0), 50); // Red
      }
      if (Light1_State == 0){
        colorWipe(strip.Color(0, 0, 0), 50);
      }      
    }
   if (subscription == &Screen){
    Serial.print(F("Gots: "));
    Serial.println((char *)Screen.lastread);
    String text = (char *)Screen.lastread;
  display.clearDisplay();
  display.display();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(10, 20);
  display.println(utf8rus(text));
  display.display();      // Show initial text
   }
   if (subscription == &Motion){
    Serial.print(F("Got: "));
    Serial.println((char *)Motion.lastread);
    motionstate = atoi((char *)Motion.lastread);  // convert to a number
   }

   mqtt.ping(); 
  }
        long now = millis();
  if (motionstate == 1){
  if (digitalRead(MotionPin) == HIGH) // перевіряємо що на пін подається сигнал високого рівня, тобто детектовано рух
  {
  if (now > (lastSend + minSecsBetweenEmails * 1000)) //додаткова перевірка від надлишкового спрацювання
  {   Move.publish(1);
     display.clearDisplay();
  display.display();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(10, 20);
  display.println("ALARM!");
  display.display();      // Show initial text
    lastSend = now;
  }
  }
  }
   
  
}
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
  
}


void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

String utf8rus(String source)
{
  int i,k;
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };

  k = source.length(); i = 0;

  while (i < k) {
    n = source[i]; i++;

    if (n >= 0xC0) {
      switch (n) {
        case 0xD0: {
          n = source[i]; i++;
          if (n == 0x81) { n = 0xA8; break; }
          if (n >= 0x90 && n <= 0xBF) n = n + 0x30;
          break;
        }
        case 0xD1: {
          n = source[i]; i++;
          if (n == 0x91) { n = 0xB8; break; }
          if (n >= 0x80 && n <= 0x8F) n = n + 0x70;
          break;
        }
      }
    }
    m[0] = n; target = target + String(m);
  }
return target;
}


