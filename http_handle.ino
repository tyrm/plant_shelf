void handleClock() {
  digitalWrite(led, 1);
  if (server.method() == HTTP_GET) {
    DateTime now = rtc.now();
    StaticJsonBuffer<256> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    
    root["year"]   = now.year();
    root["month"]  = now.month();
    root["day"]    = now.day();
    root["hour"]   = now.hour();
    root["minute"] = now.minute();
    root["second"] = now.second();
    
    int bufferSize = root.measureLength() + 1;
    char responseBuffer[bufferSize];
    root.printTo(responseBuffer, bufferSize);
    
    server.send(200, "text/json", responseBuffer);
  }
  else {
    for (uint8_t i=0; i<server.args(); i++){
      if (server.argName(i) == "plain") {
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& root = jsonBuffer.parseObject(server.arg(i));

        if (!root.success()) {
          Serial.println("parseObject() failed");
          return;
        }

        int jhour = root["hour"];
        
        Serial.println(jhour, DEC);
      }
    }
    
    server.send(200, "text/plain", "clock post");
  }
  digitalWrite(led, 0);
}

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp8266!");
  digitalWrite(led, 0);
}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}
