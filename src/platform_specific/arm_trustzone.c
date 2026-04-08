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


#include "config.h"

#if USE_TRUSTZONE
// Placeholder for TrustZone specific implementations
// These would interface with the Secure World (EL3/EL1)

int secure_world_check(void) {
    // Return 1 if in Secure World, 0 otherwise
    return 1; 
}

int secure_crypto_init(void) {
    return 0;
}

crypto_result_t secure_crypto_hash(const uint8_t *data, size_t len, uint8_t *out) {
    // Call hardware accelerator
    return CRYPTO_OK;
}

crypto_result_t secure_crypto_sign(const uint8_t *data, size_t len, uint8_t *sig, size_t *sig_len) {
    return CRYPTO_OK;
}

int secure_crypto_verify(const uint8_t *data, size_t len, const uint8_t *sig, size_t sig_len) {
    return 1;
}

void secure_crypto_cleanup(void) {}

uint32_t secure_crypto_generate_key(key_type_t type) {
    return 0x12345678; // Dummy ID
}

int secure_keystore_hw_init(void) { return 0; }
int secure_crypto_use_key(uint32_t id, uint8_t *buf, size_t len, uint8_t *res, size_t *res_len) { return 0; }
#endif
