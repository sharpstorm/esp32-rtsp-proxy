#pragma once

#include "Arduino.h"
#include "WiFi.h"

class WifiManager {
 public:
  WifiManager();

  // As station
  bool isWifiConfigured();
  bool connectToWifi();
  bool isConnectedToWifi();
  void disconnectWifi();
  String getConfiguredSsid();
};

extern WifiManager wifiManager;
