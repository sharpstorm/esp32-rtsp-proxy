#include "md5.h"

#include "mbedtls/md5.h"

String MD5::digest(String input) {
  unsigned char digest[16];
  mbedtls_md5_context ctx;

  mbedtls_md5_init(&ctx);
  mbedtls_md5_starts_ret(&ctx);
  mbedtls_md5_update(&ctx, (const unsigned char*)input.c_str(), input.length());
  mbedtls_md5_finish(&ctx, digest);
  mbedtls_md5_free(&ctx);

  String hash = "";
  for (int i = 0; i < 16; i++) {
    if (digest[i] < 0x10) hash += "0";
    hash += String(digest[i], HEX);
  }
  return hash;
}
