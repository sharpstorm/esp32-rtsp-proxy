#include "sha1.h"

#include "Base64.h"
#include "mbedtls/sha1.h"

String SHA1::digestBase64(const uint8_t* data, size_t length) {
  unsigned char hash[20];
  mbedtls_sha1_context ctx;

  mbedtls_sha1_init(&ctx);
  mbedtls_sha1_starts(&ctx);
  mbedtls_sha1_update(&ctx, data, length);
  mbedtls_sha1_finish(&ctx, hash);
  mbedtls_sha1_free(&ctx);

  return Base64::toBase64((char*)hash, 20);
}
