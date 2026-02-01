#pragma once

#include <ESPAsyncWebServer.h>

using namespace std;

class WebSocketProxyHandler {
 public:
  static void onWsEvent(AsyncWebSocket* server, AsyncWebSocketClient* client,
                        AwsEventType type, void* arg, uint8_t* data,
                        size_t len);
};
