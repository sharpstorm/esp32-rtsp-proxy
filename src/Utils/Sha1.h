#pragma once

#include "Arduino.h"

class SHA1 {
 public:
  static String digestBase64(const uint8_t* data, size_t length);
};
