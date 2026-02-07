#include "Base64.h"

String Base64::toBase64(String input) {
  return toBase64(input.c_str(), input.length());
}

String Base64::toBase64(const char* input, const int length) {
  const char b64[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  String output = "";
  int val = 0, valb = -6;

  for (int i = 0; i < length; i++) {
    val = (val << 8) + input[i];
    valb += 8;
    while (valb >= 0) {
      output += b64[(val >> valb) & 0x3F];
      valb -= 6;
    }
  }
  if (valb > -6) output += b64[((val << 8) >> (valb + 8)) & 0x3F];
  while (output.length() % 4) output += '=';

  return output;
}

String Base64::fromBase64(String input) {
  const char b64[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  String result = "";

  int val = 0, valb = -8;
  for (char c : input) {
    if (c == '=') break;

    int pos = 0;
    for (; pos < 64; pos++) {
      if (b64[pos] == c) break;
    }
    if (pos >= 64) continue;

    val = (val << 6) + pos;
    valb += 6;

    if (valb >= 0) {
      result += char((val >> valb) & 0xFF);
      valb -= 8;
    }
  }

  return result;
}
