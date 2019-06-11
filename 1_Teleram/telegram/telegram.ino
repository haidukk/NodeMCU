#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <TelegramBot.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define BeepPin 14       // Pin for voice signal D5
#define LedPin  12       // Pin for LED D6
#define MotionPin 13     // Pin for motion sensor D7
#define ServoPin 0       // Pin for servo SD3

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, LedPin, NEO_GRB + NEO_KHZ800);

const char* ssid = "Kostik";
const char* password = "Sofik_pro";
const char BotToken[] = "785065899:AAFhxjGlhqeQwaz1CogceW3agHkxii5sP1Y";

int ledstate = 0;
int motionstate = 1;
int m2state = 1;
int minSecsBetweenEmails = 20; // 1 min
long lastSend = -minSecsBetweenEmails * 1000; // змінна для запобігання відправки декількох листів одночасно

WiFiClientSecure net_ssl;
Servo servo;
TelegramBot bot (BotToken, net_ssl);


void setup() {
  servo.attach(ServoPin);
  servo.write(0);

  strip.begin();
  strip.setBrightness(50);
  strip.show(); 
  
  pinMode(BeepPin, OUTPUT);

  pinMode(MotionPin, INPUT);
  
  digitalWrite(BeepPin, HIGH);
  Serial.begin(9600);
  
  //while (!Serial); // Wait for the Serial monitor to be opened
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  display.cp437(true);
  display.clearDisplay();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(10, 20);
  display.println("Haiduk");
  display.display();
    delay(1000);
  
  
  // attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
   ArduinoOTA.setHostname("TelegramBot-esp8266");
   ArduinoOTA.setPassword((const char *)"Haiduk");
   ArduinoOTA.begin();
   
  bot.begin();
  message b = bot.getUpdates();
   bot.sendMessage(b.chat_id="264865872","Esp has started (Пристрій ввімкнено)");
   display.clearDisplay();

}

void loop() {
      ArduinoOTA.handle();
    message m = bot.getUpdates(); // Read new messages
 //   m.text = utf8rus(m.text);
////////////////////////////////////////////////////////////////////////////
    if (m.text.equals("/beep")){
      digitalWrite(BeepPin, LOW);
      delay(500);
      digitalWrite(BeepPin, HIGH);
      bot.sendMessage(m.chat_id, "Звуковий сигнал подано");
    }
///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
   if (m.text == "/servo") {
    servo.write(180);
    delay(1000);
    servo.write(0); 
    bot.sendMessage(m.chat_id, "Сервопривід рухається");
    m.text = "/beep";
   }
   
    if (m.text == "/led"){


    ledstate = ledstate + 1;
    m.text = "/beep";
    bot.sendMessage(m.chat_id, "Led змінив свій стан");
    }

    if (ledstate % 2 == 0){
    colorWipe(strip.Color(0, 0, 0), 50); // Red
    
    }
    else
    {
     colorWipe(strip.Color(148, 255, 0), 50); // Red
        
    }

      if (m.text == "/motion_security"){
      motionstate = motionstate + 1;
      m.text = "/beep";
      }
      
      if ( motionstate > m2state && motionstate % 2 == 0) {
        bot.sendMessage(m.chat_id, "Motion sensor on");
      }
       else if (motionstate > m2state && motionstate % 2 == 1){
        bot.sendMessage(m.chat_id, "Motion sensor off");
       }

      m2state = motionstate;
       long now = millis(); // початок відрахунку часу
      if (motionstate % 2 == 0){
      if (digitalRead(MotionPin) == HIGH) // перевіряємо що на пін подається сигнал високого рівня, тобто детектовано рух
      {
      if (now > (lastSend + minSecsBetweenEmails * 1000)) //додаткова перевірка від надлишкового спрацювання
      {
   
 bot.sendMessage(m.chat_id = "264865872", "Alarm! Motion has been detected!");
  Serial.println("Alarm! Motion has been detected!");
 lastSend = now;
 m.text = "/beep";
}
  }
}

if (m.text == "/clear")
 { display.clearDisplay();
  display.display();
   m.text = "/beep";
 };
   //////////////////////////////////////
    
    if ( m.text != "/beep"){
    if ( m.chat_id != 0 ){ // Checks if there are some updates
      Serial.println(m.text);
      bot.sendMessage(m.chat_id, "Відображено на екрані:\t" + m.text +"\t (як нотатка)");  // Reply to the same chat with the same text
      display.clearDisplay();
    } else {
      Serial.println("no new message");
    }
    }
///////////////////////////////////////////////////////////////////////




 if (m.text != "/beep"){
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(10, 20);
  display.println(m.text);
  display.display();      // Show initial text
 }

}
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}


