#ifndef LED_SERVER_H
#define LED_SERVER_H

#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <ESPAsyncWebServer.h>

#include "Arduino.h"
#include "LedStrip.h"
// #include "Password.h"

#define RD D0
#define BL D1
#define GN D8

class LedServer{

public:
	
	LedServer(int port);
	~LedServer();
  	
private:

  /*AsyncWebServer server_;
  LedStrip ledstrip_;*/
};

#endif
