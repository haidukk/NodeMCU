void HTTP_init(void) {
  HTTP.onNotFound(handleNotFound); // Сообщение если нет страницы. Попробуйте ввести http://192.168.0.101/restar?device=ok&test=1&led=on
  HTTP.on("/", handleRoot); // Главная страница http://192.168.0.101/
//  HTTP.on("/restart", handle_Restart); // Перезагрузка модуля по запросу вида http://192.168.0.101/restart?device=ok
  HTTP.on("/hi", helloworld);
  HTTP.on("/led_on", led_on);
  HTTP.on("/led_off", led_off);
  HTTP.on("/servo", rotate_Servo);
  HTTP.on("/beep", beep);
  HTTP.on("/clear", clear_Screen);
  HTTP.on("/display", display_on_screen);
  HTTP.on("/sensor_on", sensor_on);
  HTTP.on("/sensor_off", sensor_off);
  HTTP.on("/all_off", all_off);
  // Запускаем HTTP сервер
  HTTP.begin();

}

void helloworld(){
HTTP.send(200, "text/plain", "Hello world");
}
void all_off(){
  HTTP.send(200, "text/plain", "All off");
 display.clearDisplay();
  display.display();
  colorWipe(strip.Color(0, 0, 0), 50); // Red
 }
// Ответ если страница не найдена
void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += HTTP.uri();
  message += "\nMethod: ";
  message += (HTTP.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += HTTP.args();
  message += "\n";
  for (uint8_t i=0; i<HTTP.args(); i++){
    message += " " + HTTP.argName(i) + ": " + HTTP.arg(i) + "\n";
  }
  HTTP.send(404, "text/plain", message);
}
void sensor_on(){
  motionstate = 1;
  HTTP.send(200, "text/plain; charset=UTF-8", "Датчик руху ввімкнено");
}
void sensor_off(){
  motionstate = 0;
  HTTP.send(200, "text/plain; charset=UTF-8", "Датчик руху вимкнуто");
}
// Ответ при обращении к основной странице
void handleRoot() {
  //HTTP.send(200,"text/html", "<button>Google   <a href=\"google.com\"\">Google</a></button>");
   HTTP.send(200,"text/html;charset=UTF-8", "<a href=\"/led_on\"\">Включити світлодіод</a><br><a href=\"/led_off\"\">Виключити світлодіод</a><br><a href=\"/servo\"\">Повернути сервопривід</a><br><a href=\"/beep\"\">Подати звуковий сигнал</a><br><a href=\"/clear\"\">Очистити екран</a><br><a href=\"/hi\"\">Привітання</a><br><a href=\"/display\"\">Дисплей</a><br><a href=\"/sensor_off\"\">Вимкнути датчик руху</a><br><a href=\"/sensor_on\"\">Ввімкнути датчик руху</a><br><a href=\"/all_off\"\">Вимкнути дисплей та світлодіод</a>");
//  HTTP.send(200, "text/html", "<a href=\"http://google.com\">Google</a>");
//  HTTP.send(200, "text/plain; charset=UTF-8", "Привет, я esp8266");
//  HTTP.send(200,"text/html", "<button>Кнопка <input type=\"button\" value=\"Google\"/> <a href=\"google.com\" style=\"background: #ccc; color: #000; border: 1px solid black; padding: 5px;\">Кнопка</a>");
}

void led_on(){
  HTTP.send(200, "text/plain; charset=UTF-8", "Світлодіод ввімкнено");
    colorWipe(strip.Color(148, 255, 0), 50); // Red
}
void led_off(){
  HTTP.send(200, "text/plain; charset=UTF-8", "Світлодіод вимкнено");
  colorWipe(strip.Color(0, 0, 0), 50); // Red
}
void beep(){
      HTTP.send(200, "text/plain; charset=UTF-8", "Бiiiп!!!");
      digitalWrite(BeepPin, LOW);
      delay(500);
      digitalWrite(BeepPin, HIGH);
}
// Перезагрузка модуля по запросу вида http://192.168.0.101/restart?device=ok
/*void handle_Restart() {
  String restart = HTTP.arg("device");
  if (restart == "ok") ESP.restart();
  HTTP.send(200, "text/plain", "OK");
}*/
void rotate_Servo(){
  HTTP.send(200, "text/plain; charset=UTF-8", "Поворот сервопривода");
   servo.write(180);
    delay(1000);
    servo.write(0); 
}
void clear_Screen(){
   HTTP.send(200, "text/plain; charset=UTF-8", "Екран очищенно");
  display.clearDisplay();
  display.display();
}
void display_on_screen()
{
  display.clearDisplay();
  display.display();
  String text = HTTP.arg("text");
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(10, 20);
  display.println(text);
  if (text == "") display.println("nothing");;
  display.display();      // Show initial text
  HTTP.send(200, "text/plain; charset=UTF-8", "Ок");
}
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

int get_http(String state)
{
   HTTPClient http;
   int ret = 0;
   Serial.print("[HTTP] begin...\n");
   // configure ifttt server and url  should be HTTP only..not https!!!  (http://)  
    http.begin("http://maker.ifttt.com/trigger/Esp8266/with/key/dRTZfZq7WhZ-D9YCbZfhT0"); //HTTP
   //  http.begin("http://maker.ifttt.com/use/k_VHiSBiTbNJeu77xlW30"); //HTTP
   //If you have enabled value1 from iftt settings then uncomment below line to send value and comment out above line  
   //http.begin("http://maker.ifttt.com/trigger/door/with/key/your_key_from_Iftt/?value1="+state); //HTTP

    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();
    // httpCode will be negative on error
    if(httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET code: %d\n", httpCode);

      if(httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
      }
    } else {
        ret = -1;
        Serial.printf("[HTTP] GET failed, error: %s\n", http.errorToString(httpCode).c_str());
        delay(500); // wait for half sec before retry again
    }

    http.end();
    return ret;
} 
