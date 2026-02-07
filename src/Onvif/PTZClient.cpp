#include "PTZClient.h"

#include "../Config/ConfigManager.h"
#include "../Utils/Base64.h"
#include "../Utils/Md5.h"
#include "../Utils/Sha1.h"

const String uri = "/onvif/ptz";
const String method = "POST";

PTZClient::PTZClient() {}

void PTZClient::readConfig() {
  CameraConfig cfg = configManager.getCameraConfig();
  ip = cfg.ip;
  port = cfg.onvifPort;
  username = cfg.username;
  password = cfg.password;
}

String PTZClient::getISO8601Timestamp() {
  time_t now = time(nullptr);
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);

  char buffer[30];
  strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S.000Z", &timeinfo);
  return String(buffer);
}

String PTZClient::createPasswordDigest(String nonce64, String created) {
  String nonceDecoded = Base64::fromBase64(nonce64);

  size_t totalLen =
      nonceDecoded.length() + created.length() + password.length();
  uint8_t* combined = (uint8_t*)malloc(totalLen);

  size_t offset = 0;
  memcpy(combined + offset, nonceDecoded.c_str(), nonceDecoded.length());
  offset += nonceDecoded.length();
  memcpy(combined + offset, created.c_str(), created.length());
  offset += created.length();
  memcpy(combined + offset, password.c_str(), password.length());

  String digest = SHA1::digestBase64(combined, totalLen);
  free(combined);

  return digest;
}

String PTZClient::generateNonceBase64(size_t length) {
  uint8_t* nonce = (uint8_t*)malloc(length);
  for (size_t i = 0; i < length; i++) {
    nonce[i] = esp_random() & 0xFF;
  }

  String result = Base64::toBase64((const char*)nonce, length);
  free(nonce);
  return result;
}

String PTZClient::createSecurityHeader() {
  String nonce = generateNonceBase64(16);
  String created = getISO8601Timestamp();
  String passwordDigest = createPasswordDigest(nonce, created);

  String header =
      "<Security s:mustUnderstand=\"1\" "
      "xmlns=\"http://docs.oasis-open.org/wss/2004/01/"
      "oasis-200401-wss-wssecurity-secext-1.0.xsd\">";
  header += "<UsernameToken>";
  header += "<Username>" + username + "</Username>";
  header +=
      "<Password "
      "Type=\"http://docs.oasis-open.org/wss/2004/01/"
      "oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">";
  header += passwordDigest;
  header += "</Password>";
  header +=
      "<Nonce "
      "EncodingType=\"http://docs.oasis-open.org/wss/2004/01/"
      "oasis-200401-wss-soap-message-security-1.0#Base64Binary\">";
  header += nonce;
  header += "</Nonce>";
  header +=
      "<Created "
      "xmlns=\"http://docs.oasis-open.org/wss/2004/01/"
      "oasis-200401-wss-wssecurity-utility-1.0.xsd\">";
  header += created;
  header += "</Created>";
  header += "</UsernameToken>";
  header += "</Security>";

  return header;
}

void PTZClient::ptzMove(float x, float y, float z) {
  String soapBody =
      "<tptz:ContinuousMove>"
      "<tptz:ProfileToken>profile_1</tptz:ProfileToken>"
      "<tptz:Velocity>"
      "<tptz:PanTilt x=\"" +
      String(x, 2) + "\" y=\"" + String(y, 2) +
      "\" xmlns=\"http://www.onvif.org/ver10/schema\"/>"
      "<tptz:Zoom x=\"" +
      String(z, 2) +
      "\" xmlns=\"http://www.onvif.org/ver10/schema\"/>"
      "</tptz:Velocity>"
      "</tptz:ContinuousMove>";

  sendONVIFCommand(soapBody);
  Serial.printf("PTZ Move: x=%.2f, y=%.2f\n", x, y);
}

void PTZClient::ptzStop() {
  String soapBody =
      "<tptz:Stop>"
      "<tptz:ProfileToken>profile_1</tptz:ProfileToken>"
      "<tptz:PanTilt>true</tptz:PanTilt>"
      "<tptz:Zoom>true</tptz:Zoom>"
      "</tptz:Stop>";

  sendONVIFCommand(soapBody);
  Serial.println("PTZ Stop");
}

void PTZClient::ptzHome() {
  String soapBody =
      "<tptz:GotoHomePosition>"
      "<tptz:ProfileToken>profile_1</tptz:ProfileToken>"
      "</tptz:GotoHomePosition>";

  sendONVIFCommand(soapBody);
  Serial.println("PTZ Home");
}

bool PTZClient::sendONVIFCommand(String soapBody) {
  if (!onvifClient.connect(ip.c_str(), port)) {
    Serial.println("ONVIF connection failed");
    return false;
  }

  String securityHeader = createSecurityHeader();
  String soap = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
  soap += "<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" ";
  soap += "xmlns:tptz=\"http://www.onvif.org/ver20/ptz/wsdl\" ";
  soap += "xmlns:tt=\"http://www.onvif.org/ver10/schema\">";
  soap += "<s:Header>";
  soap += securityHeader;
  soap += "</s:Header>";
  soap += "<s:Body>";
  soap += soapBody;
  soap += "</s:Body>";
  soap += "</s:Envelope>";

  String request = method + " " + uri + " HTTP/1.1\r\n";
  request += "Host: " + ip + "\r\n";
  request += "Content-Type: application/soap+xml\r\n";
  request += "Content-Length: " + String(soap.length()) + "\r\n\r\n";
  request += soap;

  onvifClient.print(request);
  onvifClient.flush();
  delay(100);
  onvifClient.stop();

  return true;
}

PTZClient ptzClient;
