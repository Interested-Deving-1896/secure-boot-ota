#include "config.h"

#if USE_TPM
// Placeholder for TPM 2.0 implementations
// Requires TSS2 library

uint32_t secure_tpm_generate_key(key_type_t type) {
    return 0x87654321;
}

int secure_tpm_use_key(uint32_t id, uint8_t *buf, size_t len, uint8_t *res, size_t *res_len) {
    return 0;
}
#endif
