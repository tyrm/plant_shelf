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

int onHour = 8;
int onMinute = 0;

int offHour = 16;
int offMinute = 0;

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

  server.on("/clock", handleClock);
  server.on("/timer", handleTimer);
  
  server.on("/inline", [](){
    server.send(200, "text/text", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
