/*
 * Шаг №1 
 * WI-FI подключение
 */
#include <ESP8266WiFi.h>        //Содержится в пакете
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>
#include <ESP8266HTTPClient.h>
#include <Servo.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define BeepPin 14       // Pin for voice signal D5
#define LedPin  12       // Pin for LED D6
#define MotionPin 13     // Pin for motion sensor D7
#define ServoPin 0       // Pin for servo SD3

int motionstate = 0;
int minSecsBetweenEmails = 20; // 1 min
long lastSend = -minSecsBetweenEmails * 1000; // змінна для запобігання відправки декількох листів одночасно


IPAddress apIP(192, 168, 4, 10);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, LedPin, NEO_GRB + NEO_KHZ800);
Servo servo;
ESP8266WebServer HTTP(80);

// Определяем переменные
String _ssid     = "Kostik"; // Для хранения SSID
String _password = "Sofik_pro"; // Для хранения пароля сети
String _ssidAP = "WiFi";   // SSID AP точки доступа
String _passwordAP = ""; // пароль точки доступа

void setup() {
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Start 1-WIFI");
  //Запускаем WIFI
  WIFIinit();
  Serial.println("Start 2-WebServer");
  HTTP_init();
  servo.attach(ServoPin);
  servo.write(0);

  strip.begin();
  strip.setBrightness(50);
  strip.show(); 
  colorWipe(strip.Color(0, 0, 0), 50);
  pinMode(BeepPin, OUTPUT);
  pinMode(MotionPin, INPUT);
  digitalWrite(BeepPin, HIGH);

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

 display.display();
  display.cp437(true);
  display.clearDisplay();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(10, 10);
  display.println("Haiduk\r");
  display.println(WiFi.localIP());
  display.display();
  delay(1000);

   ArduinoOTA.setHostname("WebServer");
   ArduinoOTA.setPassword((const char *)"Haiduk");
   ArduinoOTA.begin();
  
}

void loop() {
  HTTP.handleClient();
  ArduinoOTA.handle();
  delay(1);

}


