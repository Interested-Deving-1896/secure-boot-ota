#include "secure_boot.h"
#include "crypto_api.h"
#include <string.h>

/* Secure boot state */
static bool g_initialized = false;

secure_boot_error_t secure_boot_init(void)
{
    if (g_initialized) {
        return SECURE_BOOT_OK;
    }
   
    /* Initialize crypto subsystem */
    int crypto_result = crypto_init();
    if (crypto_result != 0) {
        return SECURE_BOOT_ERR_CRYPTO_FAILURE;
    }
   
    g_initialized = true;
    return SECURE_BOOT_OK;
}

/**
 * @brief Verify firmware signature using ECDSA P-256
 *
 * @param firmware Pointer to firmware image
 * @param size Size of firmware image
 * @param signature Pointer to ECDSA signature (64 bytes)
 * @return true if verification succeeds
 */
bool secure_boot_verify_firmware(const uint8_t *firmware, size_t size,
                                  const uint8_t *signature)
{
    /* Input validation - fail closed on invalid inputs */
    if (firmware == NULL || signature == NULL) {
        return false;
    }
   
    if (size == 0 || size > 0xFFFFFFFF) {
        return false;
    }
   
    /* Compute SHA-256 hash of firmware */
    uint8_t hash[SHA256_DIGEST_SIZE];
    int hash_result = crypto_sha256(firmware, size, hash);
    if (hash_result != 0) {
        return false;
    }
   
    /* Load public key from secure storage (OTP/eFuse) */
    uint8_t public_key[ECDSA_PUBKEY_SIZE];
    secure_boot_error_t key_result = secure_boot_load_public_key(public_key);
    if (key_result != SECURE_BOOT_OK) {
        /* Clear hash before returning */
        secure_memzero(hash, sizeof(hash));
        return false;
    }
   
    /* Verify signature using constant-time ECDSA */
    int verify_result = crypto_ecdsa_verify(public_key, hash, signature);
   
    /* Clear sensitive data from memory */
    secure_memzero(hash, sizeof(hash));
    secure_memzero(public_key, sizeof(public_key));
   
    return (verify_result == 0);
}

secure_boot_error_t secure_boot_load_public_key(uint8_t *public_key)
{
    if (public_key == NULL) {
        return SECURE_BOOT_ERR_INVALID_PARAM;
    }
   
    /*
     * In production, read from hardware secure storage:
     * - STM32: Option Bytes or OTP memory
     * - NXP: eFuse registers
     * - Nordic: UICR registers
     *
     * For demo purposes, reading from protected flash region
     */
   
    /* Placeholder: Replace with actual hardware-specific implementation */
    const uint8_t *hw_key_ptr = (const uint8_t *)0x080FFFF0; /* Example address */
   
    if (hw_key_ptr == NULL) {
        return SECURE_BOOT_ERR_KEY_NOT_FOUND;
    }
   
    memcpy(public_key, hw_key_ptr, ECDSA_PUBKEY_SIZE);
   
    /* Validate key format (basic sanity check) */
    if (public_key[0] != 0x04) { /* ECDSA uncompressed format marker */
        return SECURE_BOOT_ERR_KEY_NOT_FOUND;
    }
   
    return SECURE_BOOT_OK;
}

/**
 * @brief Constant-time memory clearing
 *
 * Prevents compiler optimizations from removing memset calls
 */
void secure_memzero(void *ptr, size_t size)
{
    volatile uint8_t *p = (volatile uint8_t *)ptr;
    while (size--) {
        *p++ = 0;
    }
}

