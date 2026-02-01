#include "Base64.h"

String Base64::toBase64(String input) {
  const char b64[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  String output = "";
  int val = 0, valb = -6;

  for (unsigned char c : input) {
    val = (val << 8) + c;
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
