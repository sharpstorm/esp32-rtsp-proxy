#pragma once

#include <WiFi.h>

enum class PTZCmd { None, Move, Stop, Home };

class PTZClient {
 public:
  PTZClient();
  void readConfig();
  void ptzMove(float x, float y, float z);
  void ptzStop();
  void ptzHome();
  void requestCmd(PTZCmd cmd, float x = 0, float y = 0, float z = 0);
  void loop();

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

  volatile PTZCmd pending = PTZCmd::None;
  float px = 0, py = 0, pz = 0;
};

extern PTZClient ptzClient;
