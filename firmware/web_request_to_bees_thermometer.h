#pragma once

#include <Arduino.h>
#include <WiFi.h>

class WebRequestToBeesThermometer {
public:
  WebRequestToBeesThermometer();
  ~WebRequestToBeesThermometer();

  void handle(WiFiClient& client);

private:
  char lineBuf[80];
  int charCount = 0;
  const String homeWebPage;
};
