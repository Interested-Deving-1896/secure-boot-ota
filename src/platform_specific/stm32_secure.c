
#include "config.h"

#if USE_STM32_SECURE
// Placeholder for STM32 Secure HAL

int HAL_SEC_Init(void) { return 0; }
int HAL_SEC_SHA256(const uint8_t *data, size_t len, uint8_t *out) { return 0; }
int HAL_SEC_ECDSA_Sign(const uint8_t *data, size_t len, uint8_t *sig, size_t *len_out) { return 0; }
int HAL_SEC_ECDSA_Verify(const uint8_t *data, size_t len, const uint8_t *sig, size_t len_sig) { return 1; }
int HAL_SEC_Keystore_Init(void) { return 0; }
uint32_t HAL_SEC_GenerateKey(key_type_t type) { return 0xAAAA; }
int HAL_SEC_UseKey(uint32_t id, uint8_t *buf, size_t len, uint8_t *res, size_t *res_len) { return 0; }
int HAL_SEC_Deinit(void) { return 0; }
#endif
