#include "RtspStreamApi.h"

#include "../Rtsp/RtspClient.h"
#include "HttpConstants.h"

void RtspStreamApi::handleStart(AsyncWebServerRequest* request) {
  if (rtspClient.startRTSPStream()) {
    request->send(200, MimeType::Plain, "Stream started");
  } else {
    request->send(500, MimeType::Plain, "RTSP connection failed");
  }
}

void RtspStreamApi::handleStop(AsyncWebServerRequest* request) {
  rtspClient.stopRTSPStream();
  request->send(200, MimeType::Plain, "Stream stopped");
}

void RtspStreamApi::handleStats(AsyncWebServerRequest* request) {
  String stats = "{";
  stats +=
      "\"streaming\":" + String(rtspClient.isStarted() ? "true" : "false") +
      ",";
  stats += "\"packets\":" + String(rtspClient.packetsReceived) + ",";
  stats += "\"bytes\":" + String(rtspClient.bytesReceived) + ",";
  stats += "\"freeHeap\":" + String(ESP.getFreeHeap());
  stats += "}";
  request->send(200, MimeType::JSON, stats);
}
