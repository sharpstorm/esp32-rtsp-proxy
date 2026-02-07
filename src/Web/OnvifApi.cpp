#include "OnvifApi.h"

#include "../Onvif/PTZClient.h"

void OnvifApi::handlePTZ(AsyncWebServerRequest* request, uint8_t* data,
                         size_t len, size_t index, size_t total) {
  String body = "";
  for (size_t i = 0; i < len; i++) {
    body += (char)data[i];
  }

  if (body.indexOf("\"move\"") > 0) {
    float x = 0, y = 0;
    int xPos = body.indexOf("\"x\":");
    int yPos = body.indexOf("\"y\":");
    if (xPos > 0)
      x = body.substring(xPos + 4, body.indexOf(",", xPos)).toFloat();
    if (yPos > 0)
      y = body.substring(yPos + 4, body.indexOf("}", yPos)).toFloat();
    ptzClient.ptzMove(x, y, 0);
  } else if (body.indexOf("\"stop\"") > 0) {
    ptzClient.ptzStop();
  } else if (body.indexOf("\"home\"") > 0) {
    ptzClient.ptzHome();
  }

  request->send(200, "text/plain", "OK");
}
