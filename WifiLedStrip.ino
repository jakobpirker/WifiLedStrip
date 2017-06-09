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
int readingIn = 0;
bool automatic = false;

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

void handleColors(){
  int r, g, b;

  if(!readQueryParameter("r", r) || !readQueryParameter("g", g) || !readQueryParameter("b", b)){
    server.send(200, "text/plain", "wrong query parameters given!");
  }
  else {
    strip.setColors(r, g, b);
    server.send(200, "text/plain", "colors changed!"); 
  }
}

void handlePower(){
  int power;

  if(!readQueryParameter("power", power)){
    server.send(200, "text/plain", "wrong query parameters given!");
  }
  else {
    if(power){
      strip.powerOn();
      server.send(200, "text/plain", "power on!");
    }
    else {
      strip.powerOff();
      server.send(200, "text/plain", "power off!");
    }  
  } 
}

void handleBrightness(){
  int brightness;

  if(!readQueryParameter("brightness", brightness)){
    server.send(200, "text/plain", "wrong query parameters given!");
  }
  else{
    server.send(200, "text/plain", "brightness changed!");
    strip.setBrightness(brightness); 
  }   
}

void setup() {
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
  server.on("/power", handlePower);
  server.on("/color", handleColors);
  server.on("/brightness", handleBrightness);

  server.on("/post", [](){
    String message = "";
    for (uint8_t i=0; i<server.args(); i++){
      message += server.arg(i);
    }
    Serial.println(message);
    char json[] = "{\"brightness\":\"50\",\"power\":1,\"r\":\"255\", \"g\":\"0\" }"; //, \"b\":\"0\" }";
    strip.parseJson(json);
    char resp[JSON_BUFFER_SIZE];
    strip.generateJson(resp, JSON_BUFFER_SIZE);
    Serial.println(resp);
    server.send(200, "text/plain", message);
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

  strip.powerOff();
}

void loop() {
  server.handleClient();
  ArduinoOTA.handle();
}

boolean readQueryParameter(String par_name, int &param){
  String str_param = server.arg(par_name);

  // parameter not found
  if(str_param == "")
    return false;
    
  // parameter found
  param = str_param.toInt();

  return true;
}

