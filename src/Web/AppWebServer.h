#pragma once

#include <Arduino.h>

// EspAsyncWebServer Imports
#include <AsyncTCP.h>
#include <WiFi.h>

#include "ArduinoJson.h"

// Web Server
#include <EspAsyncWebServer.h>

#include "AsyncJson.h"

using namespace std;

#define HTTP_PORT 80
#define WS_ROUTE "/ws"

#define BATCH_BUFFER_SIZE 8192

class AppWebServer {
 private:
  AsyncWebServer server;
  AsyncWebSocket ws;

  uint8_t batchBuffer[BATCH_BUFFER_SIZE];
  size_t batchIndex = 0;

  void flushWsBatch();

 public:
  AppWebServer();
  void start();
  void stop();
  void proxyTraffic();
  void cleanupWsClients();
};

extern AppWebServer appWebServer;
