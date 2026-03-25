#ifndef CRYPTO_UTILS_H
#define CRYPTO_UTILS_H

#include <stdint.h>
#include <stddef.h>
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CRYPTO_OK = 0,
    CRYPTO_ERR_INVALID_PARAM = -1,
    CRYPTO_ERR_VERIFICATION_FAILED = -2,
    CRYPTO_ERR_SIGNATURE_INVALID = -3,
    CRYPTO_ERR_MEMORY = -4,
    CRYPTO_ERR_INIT_FAILED = -5,
    CRYPTO_ERR_HW_ACCESS = -6,
    CRYPTO_ERR_KEY_NOT_FOUND = -7
} crypto_result_t;

typedef struct {
    char version[MAX_VERSION_LEN];
    uint64_t timestamp;
    char device_id[MAX_VERSION_LEN];
    uint8_t firmware_hash[HASH_SIZE];
    uint8_t signature[SIGNATURE_SIZE];
    uint32_t version_number;
    uint8_t flags;
} firmware_manifest_t;

typedef enum {
    CRYPTO_ALGO_SHA256 = 0,
    CRYPTO_ALGO_SHA384 = 1,
    CRYPTO_ALGO_SHA512 = 2,
    CRYPTO_ALGO_ECDSA_P256 = 3,
    CRYPTO_ALGO_ECDSA_P384 = 4
} crypto_algo_t;

crypto_result_t crypto_init(void);
crypto_result_t crypto_hash(const uint8_t *data, size_t data_len,
                           uint8_t *output, crypto_algo_t algo);
crypto_result_t crypto_sign(const uint8_t *data, size_t data_len,
                           uint8_t *signature, size_t *sig_len);
int crypto_verify(const uint8_t *data, size_t data_len,
                 const uint8_t *signature, size_t sig_len);
crypto_result_t crypto_create_manifest(const uint8_t *firmware_data,
                                       size_t firmware_len,
                                       firmware_manifest_t *manifest);
crypto_result_t crypto_verify_manifest(const firmware_manifest_t *manifest,
                                       const uint8_t *firmware_data,
                                       size_t firmware_len);
void crypto_cleanup(void);
int crypto_memcmp(const void *a, const void *b, size_t len);
void crypto_memzero(void *ptr, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* CRYPTO_UTILS_H */

