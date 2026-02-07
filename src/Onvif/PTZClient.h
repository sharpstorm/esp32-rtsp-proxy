#pragma once

#include <WiFi.h>

class PTZClient {
 public:
  PTZClient();
  void readConfig();
  void ptzMove(float x, float y, float z);
  void ptzStop();
  void ptzHome();

 private:
  String ip;
  unsigned short port;
  String username;
  String password;

  WiFiClient onvifClient;
  bool isConnected;
  String realm;
  String nonce;

  bool sendONVIFCommand(String soapBody);
  String getISO8601Timestamp();
  String createPasswordDigest(String nonce64, String created);
  String generateNonceBase64(size_t length);
  String createSecurityHeader();
};

extern PTZClient ptzClient;
