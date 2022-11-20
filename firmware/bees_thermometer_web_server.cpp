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

  webServer.on("/graph", [this](){ handleShowTemperatureGraph(); });

  webServer.onNotFound([this](){ handlePageNotFound(); });

  Serial.print("Request handler: Remaining SRAM space = ");
  Serial.println(esp_get_free_heap_size());
}

BeesThermometerWebServer::~BeesThermometerWebServer() 
{}

void BeesThermometerWebServer::handleRoot() {
  char page[2048];
  sprintf(page, "<!DOCTYPE HTML>"                                                               \
                "<html>"                                                                        \
                "  <head>"                                                                      \
                "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/>" \
                "    <meta http-equiv='refresh' content='3'/>"                                  \
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
                "  <p>Temperature: %.1f"  \
                "      <a href=\"graph\">"                                                  \
                "      <button>graph</button>"                                                     \
                "    </a>\n"    \
                "  </p>"                                                    \
                "  <p>Humidity: %.1f\n</p>"                                                       \
                "</html>",
                currentTemperature,
                currentHumidity);

  webServer.send(200, "text/html", page);
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
  
  char page[2048];
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

void BeesThermometerWebServer::setTemperature(const float newTemperature) {
  currentTemperature = newTemperature;

  temperatureLog.push_back(newTemperature);
  if (temperatureLog.size() > 100) {
    temperatureLog.pop_front();
  }
}

void BeesThermometerWebServer::setHumidity(const float newHumidity) {
  currentHumidity = newHumidity;
}

int convertTemperatureToPixels(float temperature) {
  const float minY = -50;
  const float maxY = 50;
  
  const int heightInPixels = 200;
  
  return static_cast<int>(((temperature - minY) / (maxY - minY)) * heightInPixels);
}

void BeesThermometerWebServer::handleShowTemperatureGraph() {
  String out = "";
  char temp[100];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"250\">\n";
  out += "<rect width=\"400\" height=\"200\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"black\">\n";

  const int heightInPixels = 200;
    
  for (size_t x = 1; x < temperatureLog.size(); x++) {
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", 
                  (x - 1) * 4,
                  heightInPixels - convertTemperatureToPixels(temperatureLog[x - 1]),
                  x * 4,
                  heightInPixels - convertTemperatureToPixels(temperatureLog[x]));
    out += temp;
  }
  out += "</g>\n</svg>\n";

  webServer.send(200, "image/svg+xml", out);
}
