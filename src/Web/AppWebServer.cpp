#include "AppWebServer.h"

#include "../Rtsp/RtspClient.h"
#include "ConfigApi.h"
#include "RtspStreamApi.h"
#include "StaticServeHandler.h"
#include "WifiConfigApi.h"

AppWebServer::AppWebServer() : server(HTTP_PORT), ws(WS_ROUTE) {
  server.on("/", HTTP_GET, StaticServeHandler::handleHTML);
  server.on("/jmuxer.min.js", HTTP_GET, StaticServeHandler::handleJmuxer);

  server.on("/api/config/camera", HTTP_GET, ConfigApi::getCameraConfig);
  server.on("/api/config/camera", HTTP_POST, ConfigApi::saveCameraConfig);
  server.on("/api/config/wifi", HTTP_GET, WifiConfigApi::getSavedWifiNetwork);
  server.on("/api/config/wifi", HTTP_POST, WifiConfigApi::saveWifiNetwork);
  server.on("/api/config/dnsname", HTTP_POST, ConfigApi::saveDnsName);

  server.on("/api/rtsp/start", HTTP_GET, RtspStreamApi::handleStart);
  server.on("/api/rtsp/stop", HTTP_GET, RtspStreamApi::handleStop);
  server.on("/api/rtsp/stats", HTTP_GET, RtspStreamApi::handleStats);

  server.addHandler(&ws);
}

void AppWebServer::start() { server.begin(); }

void AppWebServer::stop() { server.end(); }

void AppWebServer::cleanupWsClients() { ws.cleanupClients(); }

void AppWebServer::proxyTraffic() {
  // 1. Loop until we run out of UDP packets
  while (int packetSize = rtspClient.parseRTSPPacket()) {
    // Safety check: specific RTSP packets can be huge.
    // If a SINGLE packet is too big for our batch, skip it or increase buffer.
    if (packetSize + 2 > BATCH_BUFFER_SIZE) {
      rtspClient.dropRTSPBuffer();
      continue;
    }

    // 2. Check if adding this packet would overflow the batch buffer
    // We need 2 bytes for the length header + the packet itself
    if (batchIndex + 2 + packetSize > BATCH_BUFFER_SIZE) {
      flushWsBatch();  // Send what we have so far
    }

    // 3. Write the Length Header (2 bytes, Big Endian)
    // This tells JS: "The next N bytes belong to one packet"
    batchBuffer[batchIndex++] = (packetSize >> 8) & 0xFF;
    batchBuffer[batchIndex++] = (packetSize & 0xFF);

    // 4. Read the UDP packet directly into the batch buffer
    rtspClient.read(batchBuffer + batchIndex, packetSize);
    batchIndex += packetSize;
    rtspClient.packetsReceived++;
  }

  // 5. Loop finished? Send whatever is left in the buffer.
  if (batchIndex > 0) {
    flushWsBatch();
  }
}

void AppWebServer::flushWsBatch() {
  // Iterate through clients (usually just one)
  for (auto const& c : ws.getClients()) {
    if (c.status() == WS_CONNECTED) {
      // Check queue health before sending
      if (!c.queueIsFull()) {  // Leave some headroom
        ws.binary(c.id(), batchBuffer, batchIndex);
        rtspClient.bytesReceived += batchIndex;
      }
    }
  }
  // Reset buffer for next time
  batchIndex = 0;
}

AppWebServer appWebServer;
