#pragma once

#include "ArduinoJson.h"

// Server
#include <ESPAsyncWebServer.h>

#include "AsyncJson.h"

class ConfigApi {
 public:
  static void getCameraConfig(AsyncWebServerRequest* request);
  static void saveCameraConfig(AsyncWebServerRequest* request,
                               JsonVariant& json);
  static void saveDnsName(AsyncWebServerRequest* request, JsonVariant& json);
};
