#include "StaticServeHandler.h"

#include "HttpConstants.h"

extern const uint8_t indexHtmlFileStart[] asm("_binary_web_index_html_start");
extern const uint8_t indexHtmlFileEnd[] asm("_binary_web_index_html_end");

extern const uint8_t jmuxerJsFileStart[] asm("_binary_web_jmuxer_min_js_start");
extern const uint8_t jmuxerJsFileEnd[] asm("_binary_web_jmuxer_min_js_end");

void StaticServeHandler::handleHTML(AsyncWebServerRequest* request) {
  AsyncResponseStream* response = request->beginResponseStream(MimeType::Html);
  size_t dataLen = indexHtmlFileEnd - indexHtmlFileStart - 1;
  response->write(indexHtmlFileStart, dataLen);
  request->send(response);
}

void StaticServeHandler::handleJmuxer(AsyncWebServerRequest* request) {
  AsyncResponseStream* response =
      request->beginResponseStream(MimeType::Javascript);
  size_t dataLen = jmuxerJsFileEnd - jmuxerJsFileStart - 1;
  response->write(jmuxerJsFileStart, dataLen);
  request->send(response);
}
