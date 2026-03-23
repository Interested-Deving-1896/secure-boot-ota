#include "crypto_utils.h"
#include <string.h>
#include <time.h>

/* Mock key storage (in production, use secure hardware) */
static uint8_t g_private_key[KEY_SIZE] = {0};
static uint8_t g_public_key[KEY_SIZE] = {0};

crypto_result_t crypto_init(void) {
    /* Initialize mock keys */
    memset(g_private_key, 0x42, KEY_SIZE);
    memset(g_public_key, 0x43, KEY_SIZE);
    return CRYPTO_OK;
}

crypto_result_t crypto_hash(const uint8_t *data, size_t data_len, uint8_t *output) {
    if (!data || !output || data_len == 0) {
        return CRYPTO_ERR_INVALID_PARAM;
    }
   
    /* Mock SHA-256 implementation */
    memset(output, 0, HASH_SIZE);
    for (size_t i = 0; i < data_len; i++) {
        output[i % HASH_SIZE] ^= data[i];
    }
   
    return CRYPTO_OK;
}

crypto_result_t crypto_sign(const uint8_t *data, size_t data_len, uint8_t *signature) {
    if (!data || !signature || data_len == 0) {
        return CRYPTO_ERR_INVALID_PARAM;
    }
   
    /* Mock signature generation */
    memset(signature, 0, SIGNATURE_SIZE);
    for (size_t i = 0; i < data_len; i++) {
        signature[i % SIGNATURE_SIZE] ^= data[i];
    }
   
    return CRYPTO_OK;
}

int crypto_verify(const uint8_t *data, size_t data_len, const uint8_t *signature) {
    if (!data || !signature || data_len == 0) {
        return 0;
    }
   
    uint8_t expected_sig[SIGNATURE_SIZE];
    crypto_sign(data, data_len, expected_sig);
   
    return memcmp(signature, expected_sig, SIGNATURE_SIZE) == 0;
}

crypto_result_t crypto_create_manifest(const uint8_t *firmware_data,
                                       size_t firmware_len,
                                       firmware_manifest_t *manifest) {
    if (!firmware_data || !manifest || firmware_len == 0) {
        return CRYPTO_ERR_INVALID_PARAM;
    }
   
    memset(manifest, 0, sizeof(firmware_manifest_t));
   
    strncpy(manifest->version, FIRMWARE_VERSION, MAX_VERSION_LEN - 1);
    manifest->timestamp = (uint64_t)time(NULL);
    strncpy(manifest->device_id, DEVICE_ID, MAX_VERSION_LEN - 1);
   
    crypto_hash(firmware_data, firmware_len, manifest->firmware_hash);
    crypto_sign(manifest->firmware_hash, HASH_SIZE, manifest->signature);
   
    return CRYPTO_OK;
}

