#include "WifiManager.h"

#include "../Config/ConfigManager.h"

WifiManager::WifiManager() {}

bool WifiManager::isWifiConfigured() {
  WifiConfig config = configManager.getWifiConfig();
  return config.isConfigured;
}

String WifiManager::getConfiguredSsid() {
  WifiConfig config = configManager.getWifiConfig();
  if (!config.isConfigured) {
    return "Unconfigured WiFi";
  }
  return config.ssid;
}

bool WifiManager::connectToWifi() {
  uint64_t timeSinceConnection = millis();
  WifiConfig config = configManager.getWifiConfig();
  if (!config.isConfigured) {
    return false;
  }

  WiFi.begin(config.ssid, config.passphrase);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (millis() - timeSinceConnection > 10000) {
      return false;
    }
  }

  return true;
}

bool WifiManager::isConnectedToWifi() { return WiFi.isConnected(); }

void WifiManager::disconnectWifi() {
  WiFi.disconnect(true);
  WiFi.setSleep(true);
  WiFi.mode(WIFI_OFF);
}

WifiManager wifiManager;
