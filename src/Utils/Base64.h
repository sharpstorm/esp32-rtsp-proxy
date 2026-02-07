#pragma once

#include "Arduino.h"

class Base64 {
 public:
  static String toBase64(String);
  static String toBase64(const char*, const int);
  static String fromBase64(String input);
};
