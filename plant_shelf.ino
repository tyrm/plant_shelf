#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include <RTClib.h>
#include <ArduinoJson.h>

const char* ssid = "......";
const char* password = "......";

ESP8266WebServer server(80);
RTC_PCF8523 rtc;

const int led = 0;

const int lamp1 = 15;
const int lamp2 = 16;

const char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

int brighness1 = 255;
int brighness2 = 255;
bool lamp1_on  = false;
bool lamp2_on  = false;

int offHour      = 16;
int offMinute    = 0;
int onHour       = 8;
int onMinute     = 0;
int onBrightness = 127;

bool clockLastTrigger             = false; // false == off, true == on
unsigned long clockPreviousMillis = 0;
unsigned long clockInterval       = 60000;

void setup(void){
  // Init RTC
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  
  if (! rtc.initialized()) {
    Serial.println("RTC is NOT running!");
  }

  // Connect to Wifi
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("plant-shelf")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/brightness", handleBrightness);
  server.on("/clock", handleClock);
  server.on("/timer", handleTimer);
  
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  
  checkAlarmState();
}

void loop(void){
  server.handleClient();
  
  unsigned long currentMillis = millis();
  if(currentMillis - clockPreviousMillis > clockInterval) {
    clockPreviousMillis = currentMillis;
    checkAlarmState();
  }
}

void checkAlarmState() {
    Serial.println("Checking Alarm");
    
    DateTime now = rtc.now();
    bool desired_state = false;

    if (offHour >= now.hour() && now.hour() >= onHour) {
      Serial.print("  hour match: ");
      Serial.println(now.hour());
      if (now.hour() == onHour) {
        if (now.minute() >= onMinute) {
          Serial.print("  minute match: ");
          Serial.println(now.minute());
          desired_state = true;
        }
      }
      else if (now.hour() == offHour) {
        if (now.minute() < offMinute) {
          Serial.print("  minute match: ");
          Serial.println(now.minute());
          desired_state = true;
        }
      }
      else {
        desired_state = true;
      }
    }
    Serial.print(" desired_state: ");
    Serial.println(desired_state);
    
    if (desired_state != clockLastTrigger) {
      clockLastTrigger = desired_state;
      Serial.print(" changing state: ");
      if (desired_state == true) {
        Serial.println("[on]");
        setBrightness(onBrightness, true, true);
      }
      else {
        Serial.println("[off]");
        setLamp(false, true, true);
      }
    }
}

void setLamp(bool s, bool l1, bool l2) {
  if (l1) {
    if (lamp1_on != s) {
      lamp1_on = s;
      if (s) {
        analogWrite(lamp1, brighness1);
      }
      else {
        analogWrite(lamp1, 0);
      }
    }
  }
  
  if (l2) {
    if (lamp2_on != s) {
      lamp2_on = s;
      if (s) {
        analogWrite(lamp2, brighness2);
      }
      else {
        analogWrite(lamp2, 0);
      }
    }
  }
}

void setBrightness(int b, bool l1, bool l2) {
  if (l1) {
    brighness1=b;
    analogWrite(lamp1, b);
    lamp1_on = true;
  }
  
  if (l2) {
    brighness2=b;
    analogWrite(lamp2, b);
    lamp2_on = true;
  }
}

