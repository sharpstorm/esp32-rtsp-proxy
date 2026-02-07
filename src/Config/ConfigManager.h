#pragma once

#include "Preferences.h"

#define CONFIG_CAMERA_NAMESPACE "rtsp-config"

#define CONFIG_CAMERA_IP "cam-ip"
#define CONFIG_CAMERA_PORT "cam-port"
#define CONFIG_CAMERA_PATH "cam-path"
#define CONFIG_CAMERA_USER "cam-user"
#define CONFIG_CAMERA_PASS "cam-pass"
#define CONFIG_CAMERA_TRACK "cam-track"
#define CONFIG_CAMERA_ONVIF_PORT "cam-onvif-port"

#define WIFI_CONFIG_NAMESPACE "wifi-config"

#define WIFI_CONFIG_SSID_KEY "wifi-ssid"
#define WIFI_CONFIG_PASSPHRASE_KEY "passphrase"
#define WIFI_CONFIG_DNS_NAME_KEY "dnsname"

#define RTSP_DEFAULT_PORT 554
#define ONVIF_DEFAULT_PORT 2020

struct CameraConfig {
  String ip;
  String username;
  String password;
  String path;
  String trackName;
  uint16_t port;
  uint16_t onvifPort;
};

struct WifiConfig {
  boolean isConfigured;
  String ssid;
  String passphrase;
};

class ConfigManager {
 private:
  Preferences preferences;

 public:
  ConfigManager();

  void saveCameraConfig(const CameraConfig& cfg);
  CameraConfig getCameraConfig();

  void saveWifiConfig(String ssid, String passphrase);
  WifiConfig getWifiConfig();

  void saveDnsName(String dnsName);
  String getDnsName();
};

extern ConfigManager configManager;
