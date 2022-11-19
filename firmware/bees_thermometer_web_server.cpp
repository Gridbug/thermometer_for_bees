#include "bees_thermometer_web_server.h"

#include <stdio.h>

#include <TimeAlarms.h>

#include "global_definitions.h"


BeesThermometerWebServer::BeesThermometerWebServer() 
  : webServer(80)
{
  webServer.on("/", [this](){ handleRoot(); });
  
  webServer.on("/blue_led_on", [this](){ handleBlueLedOn(); });
  webServer.on("/blue_led_off", [this](){ handleBlueLedOff(); });

  webServer.onNotFound([this](){ handlePageNotFound(); });

  Serial.print("Request handler: Remaining SRAM space = ");
  Serial.println(esp_get_free_heap_size());
}

BeesThermometerWebServer::~BeesThermometerWebServer() 
{}

void BeesThermometerWebServer::handleRoot() {
  const String page = "<!DOCTYPE HTML>"                                                               \
                      "<html>"                                                                        \
                      "  <head>"                                                                      \
                      "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"  \
                      "  </head>"                                                                     \
                      "  <h1>Bee's thermometer</h1>"                                                  \
                      "  <p>Blue led controls: "                                                      \
                      "    <a href=\"blue_led_on\">"                                                  \
                      "      <button>ON</button>"                                                     \
                      "    </a>"                                                                      \
                      "    <a href=\"blue_led_off\">"                                                 \
                      "      <button>OFF</button>"                                                    \
                      "    </a>"                                                                      \
                      "  </p>"                                                                        \
                      "</html>";

  webServer.send(200, "text/html", page);

  // даем браузеру пользователя время, чтобы получить данные
  Alarm.delay(1);

//  webServer.close();
}

void BeesThermometerWebServer::handleBlueLedOn() {
  Serial.println("LED 1 ON");
  digitalWrite(BLUE_LED_PIN, HIGH);
  
  handleRoot();
}

void BeesThermometerWebServer::handleBlueLedOff() {
  Serial.println("LED 1 OFF");
  digitalWrite(BLUE_LED_PIN, LOW);

  handleRoot();
}

void BeesThermometerWebServer::handlePageNotFound() {
  String requestArguments;
  for (int8_t i = 0; i < webServer.args(); i++) {
    requestArguments += " " + webServer.argName(i) + ": " + webServer.arg(i) + "\n";
  }
  
  char page[2048] = {0};
  sprintf(page, "File Not Found\n"    \
                "\n"                  \
                "URI: %s\n"           \
                "Method: %s\n"        \
                "Arguments: %d\n"     \
                "%s",
                webServer.uri(),
                (webServer.method() == HTTP_GET) ? "GET" : "POST",
                webServer.args(),
                requestArguments);

  webServer.send(404, "text/plain", page);
}

void BeesThermometerWebServer::begin() {
  webServer.begin();
}

void BeesThermometerWebServer::handleClient() {
  webServer.handleClient();
}
