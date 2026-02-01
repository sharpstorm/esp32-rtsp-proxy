#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#include <functional>

#include "../Config/ConfigManager.h"

using namespace std;

typedef function<void(const String&)> LineHandler;

class RtspClient {
 public:
  RtspClient();
  bool startRTSPStream();
  void stopRTSPStream();
  int parseRTSPPacket();
  void dropRTSPBuffer();
  int read(unsigned char* buffer, size_t len);
  bool isStarted();
  bool isConnected();

  // Stats
  unsigned long packetsReceived = 0;
  unsigned long bytesReceived = 0;

 private:
  WiFiClient wifiClient;
  WiFiUDP udpClient;

  // Internal State
  CameraConfig camConfig;
  String rtspUrl;
  int rtspCSeq = 1;
  bool isStreaming = false;

  String rtspRealm = "";
  String rtspNonce = "";
  String rtspSession = "";

  bool waitRTSPResponse(LineHandler handleLine, bool continueOnError);
  bool waitRTSPResponse(LineHandler handleLine);
  bool waitRTSPResponse();
  bool rtspHandshake();
  void flushWifiClient();
  bool detectRTSPMethods();
  bool detectRTSPAuthParams();
  String computeDigestAuth(String uri, String method);
  String computeBasicAuth();
  String getAuthHeader(String uri, String method);
};

extern RtspClient rtspClient;
