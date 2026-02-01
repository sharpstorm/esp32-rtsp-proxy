#include "RtspClient.h"

#include "../Utils/Base64.h"
#include "../Utils/Md5.h"

RtspClient::RtspClient() {}

bool RtspClient::startRTSPStream() {
  Serial.println("\n=== Starting RTSP Stream ===");

  camConfig = configManager.getCameraConfig();
  if (camConfig.ip.length() == 0 || camConfig.path.length() == 0) {
    Serial.println("ERROR: RTSP target not configured");
    return false;
  }

  rtspUrl = "rtsp://" + camConfig.ip + ":" + String(camConfig.port) + "/" +
            camConfig.path;
  Serial.printf("Camera: %s\n", rtspUrl);

  if (!wifiClient.connect(camConfig.ip.c_str(), camConfig.port)) {
    Serial.println("ERROR: Cannot connect to camera");
    return false;
  }

  Serial.println("Connected to camera");

  if (!rtspHandshake()) {
    Serial.println("ERROR: RTSP handshake failed");
    wifiClient.stop();
    return false;
  }

  Serial.println("RTSP handshake complete!");
  isStreaming = true;
  packetsReceived = 0;
  bytesReceived = 0;

  return true;
}

void RtspClient::stopRTSPStream() {
  isStreaming = false;

  if (wifiClient.connected()) {
    String teardown = "TEARDOWN " + rtspUrl + " RTSP/1.0\r\n";
    teardown += "CSeq: " + String(rtspCSeq++) + "\r\n";
    teardown += "Session: " + rtspSession + "\r\n\r\n";
    wifiClient.print(teardown);

    delay(100);
    wifiClient.stop();
  }

  Serial.println("Stream stopped");
}

bool RtspClient::waitRTSPResponse() {
  return waitRTSPResponse([](String line) {}, false);
}

bool RtspClient::waitRTSPResponse(LineHandler handleLine) {
  return waitRTSPResponse(handleLine, false);
}

bool RtspClient::waitRTSPResponse(LineHandler handleLine,
                                  bool continueOnError) {
  unsigned long start = millis();

  while (millis() - start < 5000) {
    delay(10);
    if (!wifiClient.available()) {
      continue;
    }

    String statusLine = wifiClient.readStringUntil('\n');

    // Error Statuses
    if (statusLine.indexOf("401") >= 0 || statusLine.indexOf("404") >= 0) {
      Serial.println("✗ " + statusLine);
      if (!continueOnError) {
        return false;
      }

      while (wifiClient.available()) {
        String line = wifiClient.readStringUntil('\n');
        // Handle '\r\n'
        if (line.length() <= 2) break;
        handleLine(line);
      }
      return false;
    }

    // Success status
    if (statusLine.indexOf("200") >= 0) {
      Serial.println("✓ " + statusLine);

      while (wifiClient.available()) {
        String line = wifiClient.readStringUntil('\n');
        // Handle '\r\n'
        if (line.length() <= 2) break;
        handleLine(line);
      }
      return true;
    }
  }

  Serial.println("✗ Timeout");
  return false;
}

String extractSession() { return "00000000"; }

void RtspClient::flushWifiClient() {
  while (wifiClient.available()) wifiClient.read();
}

bool RtspClient::detectRTSPMethods() {
  Serial.println("Sending OPTIONS...");
  String options = "OPTIONS " + rtspUrl + " RTSP/1.0\r\n";
  options += "CSeq: " + String(rtspCSeq++) + "\r\n\r\n";
  wifiClient.print(options);
  wifiClient.flush();

  bool areMethodsDetected = false;
  LineHandler detectMethods = [&](String line) {
    if (!line.startsWith("Public:")) {
      return;
    }

    bool hasDescribe = line.indexOf("DESCRIBE") >= 0;
    bool hasSetup = line.indexOf("SETUP") >= 0;
    bool hasTeardown = line.indexOf("TEARDOWN") >= 0;
    bool hasPlay = line.indexOf("PLAY") >= 0;

    areMethodsDetected = hasDescribe && hasSetup && hasTeardown && hasPlay;
  };

  if (!waitRTSPResponse(detectMethods)) {
    return false;
  }
  return areMethodsDetected;
}

bool RtspClient::detectRTSPAuthParams() {
  Serial.println("Sending DESCRIBE (Probe)...");
  String options = "DESCRIBE " + rtspUrl + " RTSP/1.0\r\n";
  options += "CSeq: " + String(rtspCSeq++) + "\r\n\r\n";
  wifiClient.print(options);
  wifiClient.flush();

  bool useDigest = false;
  bool useBasic = false;
  String realm;
  String nonce;

  LineHandler detectAuthParams = [&](String line) {
    if (!line.startsWith("WWW-Authenticate:")) {
      return;
    }

    bool isDigest = line.indexOf("Digest") >= 0;
    int realmStart = line.indexOf("realm=\"") + 7;
    int realmEnd = line.indexOf("\"", realmStart);
    if (realmStart > 6 && realmEnd > realmStart) {
      realm = line.substring(realmStart, realmEnd);
    }

    if (!isDigest) {
      useBasic = true;
      Serial.println("*** Basic Auth detected");
      return;
    }

    // Extract nonce
    int nonceStart = line.indexOf("nonce=\"") + 7;
    int nonceEnd = line.indexOf("\"", nonceStart);
    if (nonceStart > 6 && nonceEnd > nonceStart) {
      nonce = line.substring(nonceStart, nonceEnd);
    }

    useDigest = true;
    Serial.println("*** Digest Auth detected");
  };

  waitRTSPResponse(detectAuthParams, true);

  if (!useDigest && !useBasic) {
    Serial.println("*** Could not detect AUTH strategy");
    return false;
  }

  rtspRealm = realm;
  if (useDigest) {
    rtspNonce = nonce;
  }

  return true;
}

String RtspClient::computeDigestAuth(String uri, String method) {
  String ha1 = MD5::digest(camConfig.username + ":" + rtspRealm + ":" +
                           camConfig.password);
  String ha2 = MD5::digest(method + ":" + uri);
  String response = MD5::digest(ha1 + ":" + rtspNonce + ":" + ha2);

  String digestAuth =
      "Authorization: Digest username=\"" + camConfig.username + "\", ";
  digestAuth += "realm=\"" + rtspRealm + "\", ";
  digestAuth += "nonce=\"" + rtspNonce + "\", ";
  digestAuth += "uri=\"" + rtspUrl + "\", ";
  digestAuth += "response=\"" + response + "\"\r\n";

  return digestAuth;
}

String RtspClient::computeBasicAuth() {
  return "Authorization: Basic " +
         Base64::toBase64(camConfig.username + ":" + camConfig.password) +
         "\r\n";
}

String RtspClient::getAuthHeader(String uri, String method) {
  return rtspNonce.length() > 0 ? computeDigestAuth(uri, method)
                                : computeBasicAuth();
}

bool RtspClient::rtspHandshake() {
  rtspCSeq = 1;

  if (!detectRTSPMethods()) {
    return false;
  }

  if (!detectRTSPAuthParams()) {
    return false;
  }

  Serial.println("Sending DESCRIBE...");
  String describe = "DESCRIBE " + rtspUrl + " RTSP/1.0\r\n";
  describe += "CSeq: " + String(rtspCSeq++) + "\r\n";
  describe += getAuthHeader(rtspUrl, "DESCRIBE");
  describe += "Accept: application/sdp\r\n\r\n";
  wifiClient.print(describe);
  if (!waitRTSPResponse()) return false;

  Serial.println("Sending SETUP...");
  String setup =
      "SETUP " + rtspUrl + "/" + camConfig.trackName + " RTSP/1.0\r\n";
  setup += "CSeq: " + String(rtspCSeq++) + "\r\n";
  setup += getAuthHeader(rtspUrl, "SETUP");
  setup += "Transport: RTP/AVP;unicast;client_port=8000-8001\r\n\r\n";
  wifiClient.print(setup);

  if (!waitRTSPResponse([&](String line) {
        if (!line.startsWith("Session:")) {
          return;
        }

        int sessionEnd = line.indexOf(";");
        rtspSession = line.substring(9, sessionEnd);
      }))
    return false;

  Serial.println("Session: " + rtspSession);

  Serial.println("Sending PLAY...");
  String play = "PLAY " + rtspUrl + " RTSP/1.0\r\n";
  play += "CSeq: " + String(rtspCSeq++) + "\r\n";
  play += getAuthHeader(rtspUrl, "SETUP");
  play += "Session: " + rtspSession + "\r\n";
  play += "Range: npt=0.000-\r\n\r\n";
  wifiClient.print(play);

  if (!waitRTSPResponse()) return false;

  udpClient.begin(8000);
  Serial.println("UDP listener started on port 8000");

  return true;
}

int RtspClient::parseRTSPPacket() { return udpClient.parsePacket(); }

void RtspClient::dropRTSPBuffer() { udpClient.flush(); }

int RtspClient::read(unsigned char* buffer, size_t len) {
  return udpClient.read(buffer, len);
}

bool RtspClient::isStarted() { return isStreaming; }

bool RtspClient::isConnected() { return wifiClient.connected(); }

RtspClient rtspClient;
