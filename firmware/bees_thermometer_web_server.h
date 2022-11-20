#pragma once

#include <vector>
#include <deque>

#include <Arduino.h>
#include <WebServer.h>

class BeesThermometerWebServer {
public:
  BeesThermometerWebServer();
  ~BeesThermometerWebServer();

  void handleRoot();
  
  void handleBlueLedOn();
  void handleBlueLedOff();
  void handleShowTemperatureGraph();
  
  void handlePageNotFound();

  void begin();
  void handleClient();

  void setTemperature(const float newTemperature);
  void setHumidity(const float newHumidity);

private:
  WebServer webServer;
  
  float currentTemperature = 0;
  float currentHumidity = 0;

  std::deque<float> temperatureLog;
};
