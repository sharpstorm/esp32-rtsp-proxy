#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#include "Config/ConfigManager.h"
#include "Rtsp/RtspClient.h"
#include "Web/AppWebServer.h"
#include "Wifi/MDnsClient.h"

MDnsClient mdnsClient;
AppWebServer appServer;

void connectWiFi() {
  WifiConfig wifiCfg = configManager.getWifiConfig();
  if (!wifiCfg.isConfigured) {
    Serial.printf("No WiFi configured. Ending\n");
    while (true) {
      delay(10000);
    }
  }

  Serial.printf("Connecting to: %s\n", wifiCfg.ssid);
  WiFi.begin(wifiCfg.ssid, wifiCfg.passphrase);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.println("IP: " + WiFi.localIP().toString());
  } else {
    Serial.println("\nWiFi FAILED!");
  }
}

void setup() {
  // For setup, run this program once with this line
  // configManager.saveWifiConfig("ssid", "passphrase")

  Serial.begin(115200);
  delay(1000);

  Serial.println("\nESP32-C3 RTSP Bridge");
  Serial.println("====================");

  Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());

  connectWiFi();

  mdnsClient.start();
  appServer.start();

  Serial.println("\n*** Access: http://" + WiFi.localIP().toString());
}

void loop() {
  appServer.cleanupWsClients();
  if (rtspClient.isStarted() && rtspClient.isConnected()) {
    appServer.proxyTraffic();
  }
}
