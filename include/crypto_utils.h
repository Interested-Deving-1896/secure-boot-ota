#ifndef CRYPTO_UTILS_H
#define CRYPTO_UTILS_H

#include <stdint.h>
#include <stddef.h>
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Cryptographic operation result codes
 */
typedef enum {
    CRYPTO_OK = 0,
    CRYPTO_ERR_INVALID_PARAM = -1,
    CRYPTO_ERR_VERIFICATION_FAILED = -2,
    CRYPTO_ERR_SIGNATURE_INVALID = -3,
    CRYPTO_ERR_MEMORY = -4
} crypto_result_t;

/**
 * @brief Manifest structure for firmware updates
 */
typedef struct {
    char version[MAX_VERSION_LEN];
    uint64_t timestamp;
    char device_id[MAX_VERSION_LEN];
    uint8_t firmware_hash[HASH_SIZE];
    uint8_t signature[SIGNATURE_SIZE];
} firmware_manifest_t;

/**
 * @brief Initialize cryptographic subsystem
 * @return CRYPTO_OK on success
 */
crypto_result_t crypto_init(void);

/**
 * @brief Generate SHA-256 hash of data
 * @param data Input data buffer
 * @param data_len Length of input data
 * @param output Output hash buffer (must be HASH_SIZE bytes)
 * @return CRYPTO_OK on success
 */
crypto_result_t crypto_hash(const uint8_t *data, size_t data_len, uint8_t *output);

/**
 * @brief Sign data with private key
 * @param data Data to sign
 * @param data_len Length of data
 * @param signature Output signature buffer
 * @return CRYPTO_OK on success
 */
crypto_result_t crypto_sign(const uint8_t *data, size_t data_len, uint8_t *signature);

/**
 * @brief Verify signature against data
 * @param data Original data
 * @param data_len Length of data
 * @param signature Signature to verify
 * @return 1 if valid, 0 if invalid
 */
int crypto_verify(const uint8_t *data, size_t data_len, const uint8_t *signature);

/**
 * @brief Create signed manifest for firmware
 * @param firmware_data Firmware binary
 * @param firmware_len Length of firmware
 * @param manifest Output manifest structure
 * @return CRYPTO_OK on success
 */
crypto_result_t crypto_create_manifest(const uint8_t *firmware_data,
                                       size_t firmware_len,
                                       firmware_manifest_t *manifest);

#ifdef __cplusplus
}
#endif

#endif /* CRYPTO_UTILS_H */

