/*
 * This file is part of [secure-boot-ota].
 *
 * Copyright (C) [2026] [Maximilian Feldthusen].
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#include "crypto_utils.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>

#if USE_HARDWARE_SECURITY
#if USE_TRUSTZONE
#include <arm_trustzone.h>
#elif USE_TPM
#include <tss2/tss2_sys.h>
#elif USE_STM32_SECURE
#include "stm32_secure_hal.h"
#endif
#endif

static int initialized = 0;

crypto_result_t crypto_init(void) {
    if (initialized) {
        return CRYPTO_OK;
    }
   
 #if USE_HARDWARE_SECURITY
#if USE_TRUSTZONE
    if (!secure_world_check()) {
        return CRYPTO_ERR_HW_ACCESS;
    }
    if (secure_crypto_init() != 0) {
        return CRYPTO_ERR_INIT_FAILED;
    }
#elif USE_TPM
    TSS2_RC rc = Tss2_Sys_Init(NULL);
    if (rc != TSS2_RC_SUCCESS) {
        return CRYPTO_ERR_INIT_FAILED;
    }
#elif USE_STM32_SECURE
    if (HAL_SEC_Init() != HAL_OK) {
        return CRYPTO_ERR_INIT_FAILED;
    }
#endif
#else
    /* Software fallback - NOT FOR PRODUCTION */
    #warning "Using software crypto - NOT SECURE FOR PRODUCTION"
#endif
   
    initialized = 1;
    return CRYPTO_OK;
}

crypto_result_t crypto_hash(const uint8_t *data, size_t data_len,
                           uint8_t *output, crypto_algo_t algo) {
    if (!data || !output || data_len == 0) {
        return CRYPTO_ERR_INVALID_PARAM;
    }
   
    if (algo != CRYPTO_ALGO_SHA256) {
        return CRYPTO_ERR_INVALID_PARAM;
    }
   
 #if USE_HARDWARE_SECURITY
#if USE_TRUSTZONE
    return secure_crypto_hash(data, data_len, output);
#elif USE_TPM
    TPM2B_DIGEST hash = {0};
    TSS2_RC rc = Tss2_Sys_Hash(NULL, TPM2_RH_NULL,
                              (TPM2B_MAX_BUFFER*)data, data_len,
                              TPM2_ALG_SHA256, &hash, NULL);
    if (rc != TSS2_RC_SUCCESS) {
        return CRYPTO_ERR_HW_ACCESS;
    }
    memcpy(output, hash.t.buffer, HASH_SIZE);
    return CRYPTO_OK;
#elif USE_STM32_SECURE
    return HAL_SEC_SHA256(data, data_len, output);
#else
    /* Software SHA-256 implementation */
    /* In production, use a proper library like mbedTLS or OpenSSL */
    memset(output, 0, HASH_SIZE);
    for (size_t i = 0; i < data_len; i++) {
        output[i % HASH_SIZE] ^= data[i];
    }
    return CRYPTO_OK;
#endif
#else
    /* Software fallback */
    memset(output, 0, HASH_SIZE);
    for (size_t i = 0; i < data_len; i++) {
        output[i % HASH_SIZE] ^= data[i];
    }
    return CRYPTO_OK;
#endif
}

crypto_result_t crypto_sign(const uint8_t *data, size_t data_len,
                           uint8_t *signature, size_t *sig_len) {
    if (!data || !signature || !sig_len || data_len == 0) {
        return CRYPTO_ERR_INVALID_PARAM;
    }
   
    if (*sig_len < SIGNATURE_SIZE) {
        return CRYPTO_ERR_INVALID_PARAM;
    }
   
 #if USE_HARDWARE_SECURITY
#if USE_TRUSTZONE
    return secure_crypto_sign(data, data_len, signature, sig_len);
#elif USE_TPM
    TPM2B_DIGEST digest = {0};
    memcpy(digest.t.buffer, data, data_len > HASH_SIZE ? HASH_SIZE : data_len);
    digest.t.size = HASH_SIZE;
   
    TSS2B_DATA validation = {0};
    TPMT_SIGNATURE signature_out = {0};
   
    TSS2_RC rc = Tss2_Sys_Sign(NULL, TPM2_RH_NULL, &digest, &validation,
                              TPM2_ALG_NULL, &signature_out, NULL);
    if (rc != TSS2_RC_SUCCESS) {
        return CRYPTO_ERR_HW_ACCESS;
    }
   
    /* Extract ECDSA signature */
    size_t offset = 0;
    memcpy(signature, signature_out.sig.ecdsa.signatureR.t.buffer,
           signature_out.sig.ecdsa.signatureR.t.size);
    offset += signature_out.sig.ecdsa.signatureR.t.size;
    memcpy(signature + offset, signature_out.sig.ecdsa.signatureS.t.buffer,
           signature_out.sig.ecdsa.signatureS.t.size);
    *sig_len = offset + signature_out.sig.ecdsa.signatureS.t.size;
   
    return CRYPTO_OK;
#elif USE_STM32_SECURE
    return HAL_SEC_ECDSA_Sign(data, data_len, signature, sig_len);
#else
    /* Software fallback - NOT SECURE */
    memset(signature, 0x55, SIGNATURE_SIZE);
    *sig_len = SIGNATURE_SIZE;
    return CRYPTO_OK;
#endif
#else
    /* Software fallback */
    memset(signature, 0x55, SIGNATURE_SIZE);
    *sig_len = SIGNATURE_SIZE;
    return CRYPTO_OK;
#endif
}

int crypto_verify(const uint8_t *data, size_t data_len,
                 const uint8_t *signature, size_t sig_len) {
    if (!data || !signature || data_len == 0 || sig_len < SIGNATURE_SIZE) {
        return 0;
    }
   
 #if USE_HARDWARE_SECURITY
#if USE_TRUSTZONE
    return secure_crypto_verify(data, data_len, signature, sig_len);
#elif USE_TPM
    TPM2B_DIGEST digest = {0};
    memcpy(digest.t.buffer, data, data_len > HASH_SIZE ? HASH_SIZE : data_len);
    digest.t.size = HASH_SIZE;
   
    TPMT_SIGNATURE signature_in = {0};
    signature_in.sigAlg = TPM2_ALG_ECDSA;
    signature_in.sig.ecdsa.hash = TPM2_ALG_SHA256;
   
    /* Reconstruct ECDSA signature */
    size_t half = sig_len / 2;
    signature_in.sig.ecdsa.signatureR.t.size = half;
    signature_in.sig.ecdsa.signatureS.t.size = sig_len - half;
    memcpy(signature_in.sig.ecdsa.signatureR.t.buffer, signature, half);
    memcpy(signature_in.sig.ecdsa.signatureS.t.buffer, signature + half,
           sig_len - half);
   
    TSS2_RC rc = Tss2_Sys_VerifySignature(NULL, TPM2_RH_NULL, &digest,
                                          &signature_in, NULL);
    return (rc == TSS2_RC_SUCCESS) ? 1 : 0;
#elif USE_STM32_SECURE
    return HAL_SEC_ECDSA_Verify(data, data_len, signature, sig_len);
#else
    /* Software fallback */
    uint8_t expected_sig[SIGNATURE_SIZE];
    size_t sig_len_out = SIGNATURE_SIZE;
    crypto_sign(data, data_len, expected_sig, &sig_len_out);
    return crypto_memcmp(signature, expected_sig, SIGNATURE_SIZE) == 0;
#endif
#else
    /* Software fallback */
    uint8_t expected_sig[SIGNATURE_SIZE];
    size_t sig_len_out = SIGNATURE_SIZE;
    crypto_sign(data, data_len, expected_sig, &sig_len_out);
    return crypto_memcmp(signature, expected_sig, SIGNATURE_SIZE) == 0;
#endif
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
   
    /* Parse version number */
    sscanf(manifest->version, "%u", &manifest->version_number);
   
    /* Calculate hash */
    if (crypto_hash(firmware_data, firmware_len, manifest->firmware_hash,
                   CRYPTO_ALGO_SHA256) != CRYPTO_OK) {
        return CRYPTO_ERR_VERIFICATION_FAILED;
    }
   
    /* Sign the hash */
    size_t sig_len = SIGNATURE_SIZE;
    if (crypto_sign(manifest->firmware_hash, HASH_SIZE, manifest->signature,
                   &sig_len) != CRYPTO_OK) {
        return CRYPTO_ERR_SIGNATURE_INVALID;
    }
   
    manifest->flags = 0; /* Reserved for future use */
   
    return CRYPTO_OK;
}

crypto_result_t crypto_verify_manifest(const firmware_manifest_t *manifest,
                                       const uint8_t *firmware_data,
                                       size_t firmware_len) {
    if (!manifest || !firmware_data || firmware_len == 0) {
        return CRYPTO_ERR_INVALID_PARAM;
    }
   
    /* Verify hash */
    uint8_t calculated_hash[HASH_SIZE];
    if (crypto_hash(firmware_data, firmware_len, calculated_hash,
                   CRYPTO_ALGO_SHA256) != CRYPTO_OK) {
        return CRYPTO_ERR_VERIFICATION_FAILED;
    }
   
    if (crypto_memcmp(calculated_hash, manifest->firmware_hash, HASH_SIZE) != 0) {
        return CRYPTO_ERR_VERIFICATION_FAILED;
    }
   
    /* Verify signature */
    if (!crypto_verify(manifest->firmware_hash, HASH_SIZE, manifest->signature,
                      SIGNATURE_SIZE)) {
        return CRYPTO_ERR_SIGNATURE_INVALID;
    }
   
    return CRYPTO_OK;
}

void crypto_cleanup(void) {
#if USE_HARDWARE_SECURITY
#if USE_TRUSTZONE
    secure_crypto_cleanup();
#elif USE_TPM
    Tss2_Sys_Shutdown(NULL);
#elif USE_STM32_SECURE
    HAL_SEC_Deinit();
#endif
#endif
    initialized = 0;
}

int crypto_memcmp(const void *a, const void *b, size_t len) {
    const unsigned char *pa = (const unsigned char *)a;
    const unsigned char *pb = (const unsigned char *)b;
    volatile unsigned char result = 0;
   
    for (size_t i = 0; i < len; i++) {
        result |= pa[i] ^ pb[i];
    }
   
    return result;
}

void crypto_memzero(void *ptr, size_t len) {
    volatile unsigned char *p = (volatile unsigned char *)ptr;
    while (len--) {
        *p++ = 0;
    }
}
