#include "WebSocketProxyHandler.h"

void WebSocketProxyHandler::onWsEvent(AsyncWebSocket* server,
                                      AsyncWebSocketClient* client,
                                      AwsEventType type, void* arg,
                                      uint8_t* data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket[%u] connected\n", client->id());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket[%u] disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}
