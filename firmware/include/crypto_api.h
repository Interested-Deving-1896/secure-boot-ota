#ifndef CRYPTO_API_H
#define CRYPTO_API_H

#include <stdint.h>
#include <stddef.h>

/* Initialize crypto subsystem */
int crypto_init(void);

/* SHA-256 */
int crypto_sha256(const uint8_t *data, size_t len, uint8_t *digest);

/* AES-256-GCM */
int crypto_aes_gcm_encrypt(const uint8_t *key, const uint8_t *nonce,
                            const uint8_t *aad, size_t aad_len,
                            const uint8_t *plaintext, size_t plaintext_len,
                            uint8_t *ciphertext, uint8_t *tag);

int crypto_aes_gcm_decrypt(const uint8_t *key, const uint8_t *nonce,
                            const uint8_t *aad, size_t aad_len,
                            const uint8_t *ciphertext, size_t ciphertext_len,
                            const uint8_t *tag, uint8_t *plaintext);

/* ECDSA P-256 */
int crypto_ecdsa_verify(const uint8_t *public_key, const uint8_t *hash,
                         const uint8_t *signature);

/* Random number generation */
int crypto_random_bytes(uint8_t *buffer, size_t len);

#endif /* CRYPTO_API_H */

