#include "web_request_to_bees_thermometer.h"

#include <TimeAlarms.h>

// указываем пины, к которым подключены светодиоды
#define LED_BLUE 2


WebRequestToBeesThermometer::WebRequestToBeesThermometer() 
  : homeWebPage("<!DOCTYPE HTML>"                                           \
                "<html>"                                                    \
                "  <head>"                                                  \
                "    <meta name=\"viewport\" content=\"width=device-width," \
                "    initial-scale=1\">"                                    \
                "  </head>"                                                 \
                "  <h1>ESP32 - Web Server</h1>"                             \
                "  <p>BLUE LED"                                             \
                "    <a href=\"on1\">"                                      \
                "      <button>ON</button>"                                 \
                "    </a>&nbsp;"                                            \
                "    <a href=\"off1\">"                                     \
                "      <button>OFF</button>"                                \
                "    </a>"                                                  \
                "  </p>"                                                    \
                "</html>")
{}

WebRequestToBeesThermometer::~WebRequestToBeesThermometer() 
{}

void WebRequestToBeesThermometer::handle(WiFiClient& client) {
  memset(lineBuf, 0, sizeof(lineBuf));
  charCount = 0;
  
  // HTTP-запрос заканчивается пустой строкой
  boolean currentLineIsBlank = true;
  
  while (client.connected()) {
      if (client.available()) {
          char c = client.read();
          Serial.write(c);
          // считываем HTTP-запрос
          lineBuf[charCount] = c;
          if (charCount < sizeof(lineBuf) - 1) {
              charCount++;
          }
          // на символ конца строки отправляем ответ
          if (c == '\n' && currentLineIsBlank) {
              // отправляем стандартный заголовок HTTP-ответа
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              // тип контента: text/html
              client.println("Connection: close");
              // после отправки ответа связь будет отключена
              client.println();
              // формируем веб-страницу 
              
              client.println(homeWebPage);
              
              Serial.print("Request handler: Remaining SRAM space = ");
              Serial.println(esp_get_free_heap_size());
              break;
          }
          if (c == '\n') {
              // анализируем буфер на наличие запросов 
              // если есть запрос, меняем состояние светодиода
              currentLineIsBlank = true;
              if (strstr(lineBuf, "GET /on1") > 0) {
                  Serial.println("LED 1 ON");
                  digitalWrite(LED_BLUE, HIGH);
              } else if (strstr(lineBuf, "GET /off1") > 0) {
                  Serial.println("LED 1 OFF");
                  digitalWrite(LED_BLUE, LOW);
              }
              // начинаем новую строку
              currentLineIsBlank = true;
              memset(lineBuf, 0, sizeof(lineBuf));
              charCount = 0;
          } else if (c != '\r') {
              // в строке попался новый символ
              currentLineIsBlank = false;
          }
      }
  }
  // даем веб-браузеру время, чтобы получить данные
  Alarm.delay(1);
  // закрываем соединение
  client.stop();
  Serial.println("client disconnected"); 
}


static const String homeWebPage = "<!DOCTYPE HTML>" \
                                "<html>"    \
                                "  <head>"  \
                                "    <meta name=\"viewport\" content=\"width=device-width," \
                                "    initial-scale=1\">"    \
                                "  </head>" \
                                "  <h1>ESP32 - Web Server</h1>" \
                                "  <p>BLUE LED" \
                                "    <a href=\"on1\">"  \
                                "      <button>ON</button>" \
                                "    </a>&nbsp;"    \
                                "    <a href=\"off1\">" \
                                "      <button>OFF</button>"    \
                                "    </a>"  \
                                "  </p>"    \
                                "</html>";
