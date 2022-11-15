#include <WiFi.h>

#include "esp_log.h"
#include "esp_system.h"
#include "time.h"
#include <vector>
#include <TimeAlarms.h>

#include "web_request_to_bees_thermometer.h"

// указываем пины, к которым подключены светодиоды
#define LED_BLUE 2

const char* ssid = "AndroidAPEB55";
const char* password = "pvtq4802";

std::vector<float> localTemperatureLog;

// инициализируем сервер на 80 порте
WiFiServer server(80);

const char* ntpServer1 = "ntp7.ntp-servers.net";
const char* ntpServer2 = "ntp6.ntp-servers.net";
const long  gmtOffset_sec = 7 * 3600;
const int   daylightOffset_sec = 0;

uint32_t measuringInterval_sec = 5;

hw_timer_t *My_timer = NULL;

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

void measureTemperature(){
  digitalWrite(LED_BLUE, !digitalRead(LED_BLUE));
  printLocalTime();
}
 
void setup() {
    pinMode(LED_BLUE, OUTPUT);
    
    // инициализируем монитор порта
    Serial.begin(115200);

    setTime(8,29,0,1,1,11);

    //Настройка часов
    sntp_set_time_sync_notification_cb( timeAvailable );
    //sntp_servermode_dhcp(1);
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);

//    My_timer = timerBegin(0, 80, true);
//    timerAttachInterrupt(My_timer, &onTimer, true);
//    timerAlarmWrite(My_timer, measuringIntervalInMicroseconds, true);
//    timerAlarmEnable(My_timer); //Just Enable

    Alarm.timerRepeat(measuringInterval_sec, measureTemperature);
    
    // подключаемся к Wi-Fi сети
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");  
    Serial.println(ssid);
    
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
    
    server.begin();
}

void loop() {
    // анализируем канал связи на наличие входящих клиентов
    WiFiClient client = server.available();
    if (client) {
        Serial.println("New client");  
        
        WebRequestToBeesThermometer newWebRequest;
        newWebRequest.handle(client);
    }
    Alarm.delay(0);
}
