#pragma once

#include "ArduinoJson.h"

// Server
#include <ESPAsyncWebServer.h>

#include "AsyncJson.h"

class RtspStreamApi {
 public:
  static void handleStart(AsyncWebServerRequest* request);
  static void handleStop(AsyncWebServerRequest* request);
  static void handleStats(AsyncWebServerRequest* request);
};
