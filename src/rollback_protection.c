#include "rollback_protection.h"
#include <string.h>
#include <time.h>
#include "crypto_utils.h"
#include "secure_keystore.h"

static int rb_initialized = 0;

int rollback_init(rollback_protection_t *rb) {
    if (!rb) {
        return -1;
    }
   
    memset(rb, 0, sizeof(rollback_protection_t));
   
    /* Try to load from persistent storage */
    uint32_t stored_version = 0;
    /* In production: read from secure non-volatile storage */
    /* For demo: simulate */
   
    rb->version_counter = stored_version;
    rb->last_updated = (uint64_t)time(NULL);
    rb->flags = 0;
   
    rb_initialized = 1;
    return 0;
}

int rollback_check_and_increment(rollback_protection_t *rb,
                                uint32_t min_version) {
    if (!rb || !rb_initialized) {
        return -1;
    }
   
    /* Prevent rollback attack */
    if (rb->version_counter < min_version) {
        return -2; /* Rollback detected! */
    }
   
    /* Increment counter */
    rb->version_counter = min_version;
    rb->last_updated = (uint64_t)time(NULL);
   
    /* Sign the updated counter */
    if (rollback_sign(rb) != 0) {
        return -3;
    }
   
    /* In production: write to secure non-volatile storage */
    /* secure_storage_write(SECURE_STORAGE_ROLLBACK, rb, sizeof(rollback_protection_t)); */
   
    return 0;
}

int rollback_get_current_version(rollback_protection_t *rb,
                                uint32_t *version) {
    if (!rb || !version || !rb_initialized) {
        return -1;
    }
   
    *version = rb->version_counter;
    return 0;
}

int rollback_verify_signature(rollback_protection_t *rb) {
    if (!rb) {
        return 0;
    }
   
    /* Verify signature using hardware key */
    secure_key_handle_t key;
    if (secure_keystore_get_key_by_type(NULL, KEY_TYPE_ROOT_OF_TRUST, &key) != 0) {
        return 0;
    }
   
    /* Create data to verify (everything except signature) */
    uint8_t data[sizeof(rollback_protection_t) - 64];
    memcpy(data, rb, sizeof(rollback_protection_t) - 64);
   
    return crypto_verify(data, sizeof(data), rb->signature, 64);
}

int rollback_sign(rollback_protection_t *rb) {
    if (!rb) {
        return -1;
    }
   
    /* Create data to sign (everything except signature) */
    uint8_t data[sizeof(rollback_protection_t) - 64];
    memcpy(data, rb, sizeof(rollback_protection_t) - 64);
   
    /* Sign with hardware key */
    secure_key_handle_t key;
    if (secure_keystore_get_key_by_type(NULL, KEY_TYPE_ROOT_OF_TRUST, &key) != 0) {
        return -2;
    }
   
    size_t sig_len = 64;
    if (crypto_sign(data, sizeof(data), rb->signature, &sig_len) != CRYPTO_OK) {
        return -3;
    }
   
    return 0;
}

void rollback_cleanup(rollback_protection_t *rb) {
    if (!rb) {
        return;
    }
   
    crypto_memzero(rb, sizeof(rollback_protection_t));
    rb_initialized = 0;
}

