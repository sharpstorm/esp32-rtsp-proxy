#include "ConfigApi.h"

#include "../Config/ConfigManager.h"
#include "HttpConstants.h"

void ConfigApi::getCameraConfig(AsyncWebServerRequest* request) {
  CameraConfig config = configManager.getCameraConfig();

  // Password is sensitive so we don't want to expose it.
  AsyncJsonResponse* response = new AsyncJsonResponse();
  JsonObject root = response->getRoot().to<JsonObject>();
  root["ip"] = config.ip;
  root["port"] = config.port;
  root["path"] = config.path;
  root["track"] = config.trackName;
  root["user"] = config.username;
  response->setLength();
  request->send(response);
}

void ConfigApi::saveCameraConfig(AsyncWebServerRequest* request,
                                 JsonVariant& jsonBody) {
  JsonObject reqBody = jsonBody.as<JsonObject>();
  if (reqBody["ip"].isUnbound() || reqBody["port"].isUnbound() ||
      reqBody["path"].isUnbound() || reqBody["user"].isUnbound() ||
      reqBody["pass"].isUnbound() || reqBody["track"].isUnbound()) {
    request->send(400, MimeType::JSON, "{ \"success\": false }");
    return;
  }

  CameraConfig cfg;
  cfg.ip = reqBody["ip"].as<String>();
  cfg.port = reqBody["port"].as<uint16_t>();
  cfg.path = reqBody["path"].as<String>();
  cfg.username = reqBody["user"].as<String>();
  cfg.password = reqBody["pass"].as<String>();
  cfg.trackName = reqBody["track"].as<String>();
  configManager.saveCameraConfig(cfg);

  request->send(200, MimeType::JSON, "{ \"success\": true }");
}

void ConfigApi::saveDnsName(AsyncWebServerRequest* request,
                            JsonVariant& jsonBody) {
  JsonObject reqBody = jsonBody.as<JsonObject>();
  if (reqBody["dnsname"].isUnbound()) {
    request->send(400, MimeType::JSON, "{ \"success\": false }");
    return;
  }

  configManager.saveDnsName(reqBody["dnsname"].as<String>());
  request->send(200, MimeType::JSON, "{ \"success\": true }");
  ESP.restart();
}
