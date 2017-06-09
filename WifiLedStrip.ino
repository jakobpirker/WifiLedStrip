#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include "FS.h"
#include "LedStrip.h"
#include "Password.h"

const char * hostName = "LedStrip";

ESP8266WebServer server(80);

#define RD D0
#define BL D1
#define GN D8

LedStrip strip(RD, GN, BL, true);

void handleRoot() {  
  File f = SPIFFS.open("/index.html", "r");
  if (!f) {
      server.send(200, "text/plain", "problem reading the index-file!");
  }
  else {
    String tmp = f.readString();
    String repl = "$scope.url = 'http://"; 
    repl +=  WiFi.localIP().toString();
    repl += "';";
    Serial.println(repl);
    tmp.replace("//placeholder#1", repl);
    server.send(200, "text/html", tmp);
    f.close();
  }  
}

void handleNotFound(){
  server.send(404, "text/plain", "File Not Found");
}

void setup(){
  Serial.begin(115200);

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

  server.on("/", handleRoot);
  server.on("/leds", HTTP_GET, []() {
    char json[JSON_BUFFER_SIZE];
    strip.generateJson(json, JSON_BUFFER_SIZE);
    String tmp(json);
    server.send(200, "text/plain", tmp);
  });

  server.on("/leds", HTTP_POST, [](){    
    char json[JSON_BUFFER_SIZE];
    server.arg("plain").toCharArray(json, JSON_BUFFER_SIZE); 
    strip.parseJson(json);
    server.send(200, "text/plain");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  
  if(SPIFFS.begin())
    Serial.println("File-system started succsessfully!");
  else
    Serial.println("Couldn't start file-system!");

  ArduinoOTA.setHostname(hostName);
  ArduinoOTA.begin();
}

void loop(){
  server.handleClient();
  ArduinoOTA.handle();
}

