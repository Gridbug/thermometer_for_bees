#include <WiFi.h>

#include <vector>

#include "esp_log.h"
#include "esp_system.h"
#include "time.h"
#include <TimeAlarms.h>
#include "DHT.h"
#include <ESPmDNS.h>

#include "bees_thermometer_web_server.h"
#include "global_definitions.h"


const char* ssid = "AndroidAPEB55";
const char* password = "pvtq4802";

std::vector<float> localTemperatureLog;

BeesThermometerWebServer* webServer = nullptr;

const char* ntpServer1 = "ntp7.ntp-servers.net";
const char* ntpServer2 = "ntp6.ntp-servers.net";
const long  gmtOffset_sec = 7 * 3600;
const int   daylightOffset_sec = 0;

uint32_t measuringInterval_sec = 5;

hw_timer_t *My_timer = NULL;

DHT dht22_sensor(DHT22_SENSOR_PIN, DHT22);

void printLocalTime() {
  struct tm timeinfo;
  
  if(!getLocalTime(&timeinfo)){
    Serial.println("No time available (yet)");
    return;
  }
  
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

// Callback function (get's called when time adjusts via NTP)
void timeAvailable(struct timeval *t) {
  Serial.println("Got time adjustment from NTP!");
  printLocalTime();
}

void measureTemperature() {
//  digitalWrite(BLUE_LED_PIN, !digitalRead(BLUE_LED_PIN));
  printLocalTime();
}
 
void setup() {
    // инициализируем монитор порта
    Serial.begin(115200);

    pinMode(BLUE_LED_PIN, OUTPUT);

    setTime(8, 29, 0, 1, 1, 11);

    //Настройка часов
    sntp_set_time_sync_notification_cb( timeAvailable );
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);

    Alarm.timerRepeat(measuringInterval_sec, measureTemperature);
    
    // подключаемся к Wi-Fi сети
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");  
    Serial.println(ssid);

//    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while(WiFi.status() != WL_CONNECTED) {
        Alarm.delay(500);
        Serial.print(".");
    }
    
    Serial.println("");
    Serial.println("Wi-Fi connected");  
    Serial.println("IP-address: "); 
    
    // пишем IP-адрес в монитор порта   
    Serial.println(WiFi.localIP());

    //кэш на неделю измерений каждые 30 секунд
    localTemperatureLog.reserve(20160);
    localTemperatureLog.push_back(10);

    webServer = new BeesThermometerWebServer;
    webServer->begin();

    dht22_sensor.begin();

    if (!MDNS.begin("gradusnik")) {
        Serial.println("MDNS responder started for ");
    }
}

void loop() {
    // анализируем канал связи на наличие входящих клиентов
    webServer->handleClient();
    
    Alarm.delay(2);
}
