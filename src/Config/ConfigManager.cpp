#include "ConfigManager.h"

ConfigManager::ConfigManager() {}

void ConfigManager::saveCameraConfig(const CameraConfig& cfg) {
  preferences.begin(CONFIG_CAMERA_NAMESPACE, false);
  preferences.putString(CONFIG_CAMERA_IP, cfg.ip);
  preferences.putUShort(CONFIG_CAMERA_PORT, cfg.port);
  preferences.putUShort(CONFIG_CAMERA_ONVIF_PORT, cfg.onvifPort);
  preferences.putString(CONFIG_CAMERA_PATH, cfg.path);
  preferences.putString(CONFIG_CAMERA_TRACK, cfg.trackName);
  preferences.putString(CONFIG_CAMERA_USER, cfg.username);
  preferences.putString(CONFIG_CAMERA_PASS, cfg.password);
  preferences.end();
}

CameraConfig ConfigManager::getCameraConfig() {
  CameraConfig cfg;
  preferences.begin(CONFIG_CAMERA_NAMESPACE, true);
  cfg.ip = preferences.getString(CONFIG_CAMERA_IP, "");
  cfg.port = preferences.getUShort(CONFIG_CAMERA_PORT, RTSP_DEFAULT_PORT);
  cfg.onvifPort =
      preferences.getUShort(CONFIG_CAMERA_ONVIF_PORT, ONVIF_DEFAULT_PORT);
  cfg.path = preferences.getString(CONFIG_CAMERA_PATH, "");
  cfg.trackName = preferences.getString(CONFIG_CAMERA_TRACK, "");
  cfg.username = preferences.getString(CONFIG_CAMERA_USER, "");
  cfg.password = preferences.getString(CONFIG_CAMERA_PASS, "");
  preferences.end();

  return cfg;
}

void ConfigManager::saveWifiConfig(String ssid, String passphrase) {
  preferences.begin(WIFI_CONFIG_NAMESPACE, false);
  preferences.putString(WIFI_CONFIG_SSID_KEY, ssid);
  preferences.putString(WIFI_CONFIG_PASSPHRASE_KEY, passphrase);
  preferences.end();
}

WifiConfig ConfigManager::getWifiConfig() {
  preferences.begin(WIFI_CONFIG_NAMESPACE, true);
  String ssid = preferences.getString(WIFI_CONFIG_SSID_KEY, "");
  String passphrase = preferences.getString(WIFI_CONFIG_PASSPHRASE_KEY, "");
  preferences.end();
  bool isValid = ssid.length() > 0;

  return WifiConfig{isValid, ssid, passphrase};
}

void ConfigManager::saveDnsName(String dnsName) {
  preferences.begin(WIFI_CONFIG_NAMESPACE, false);
  preferences.putString(WIFI_CONFIG_DNS_NAME_KEY, dnsName);
  preferences.end();
}

String ConfigManager::getDnsName() {
  preferences.begin(WIFI_CONFIG_NAMESPACE, true);
  String dnsName = preferences.getString(WIFI_CONFIG_DNS_NAME_KEY, "esp-cam");
  preferences.end();
  return dnsName;
}

ConfigManager configManager;
