#pragma once

#include <Arduino.h>
#include <WebServer.h>

class BeesThermometerWebServer {
public:
  BeesThermometerWebServer();
  ~BeesThermometerWebServer();

  void handleRoot();
  void handleBlueLedOn();
  void handleBlueLedOff();
  void handlePageNotFound();

  void begin();
  void handleClient();

private:
  WebServer webServer;
  
  char lineBuf[80];
  int charCount = 0;
};
