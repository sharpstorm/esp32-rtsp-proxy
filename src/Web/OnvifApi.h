#pragma once

#include "ArduinoJson.h"

// Server
#include <ESPAsyncWebServer.h>

#include "AsyncJson.h"

class OnvifApi {
 public:
  static void handlePTZ(AsyncWebServerRequest* request, uint8_t* data,
                        size_t len, size_t index, size_t total);
};
