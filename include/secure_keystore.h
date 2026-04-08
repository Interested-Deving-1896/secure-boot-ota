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


#ifndef SECURE_KEYSTORE_H
#define SECURE_KEYSTORE_H

#include <stdint.h>
#include <stddef.h>
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    KEY_STATE_GENERATED = 0,
    KEY_STATE_PROVISIONED = 1,
    KEY_STATE_ACTIVE = 2,
    KEY_STATE_ROTATING = 3,
    KEY_STATE_REVOKED = 4,
    KEY_STATE_DESTROYED = 5
} key_state_t;

typedef enum {
    KEY_TYPE_ROOT_OF_TRUST = 0,
    KEY_TYPE_BOOTLOADER_SIGNING = 1,
    KEY_TYPE_FIRMWARE_ENCRYPTION = 2,
    KEY_TYPE_OTA_COMMUNICATION = 3,
    KEY_TYPE_DEVICE_IDENTIFICATION = 4,
    KEY_TYPE_MAX = 5
} key_type_t;

typedef struct {
    uint32_t key_id;
    key_type_t type;
    key_state_t state;
    uint8_t key_material_ref[32];
    uint64_t created_timestamp;
    uint64_t expires_timestamp;
    uint8_t usage_count;
    uint8_t rotation_count;
    uint8_t reserved[2];
} secure_key_handle_t;

typedef struct {
    secure_key_handle_t handles[MAX_KEYS];
    uint32_t next_key_id;
    uint8_t initialized;
} keystore_context_t;

int secure_keystore_init(keystore_context_t *ctx);
int secure_keystore_provision_key(keystore_context_t *ctx,
                                 key_type_t type,
                                 secure_key_handle_t *handle);
int secure_keystore_use_key(keystore_context_t *ctx,
                           secure_key_handle_t handle,
                           uint8_t *operation_buffer,
                           size_t buffer_len,
                           uint8_t *result,
                           size_t *result_len);
int secure_keystore_rotate_key(keystore_context_t *ctx,
                              secure_key_handle_t old_handle,
                              secure_key_handle_t *new_handle);
int secure_keystore_revoke_key(keystore_context_t *ctx,
                              secure_key_handle_t handle);
int secure_keystore_destroy_key(keystore_context_t *ctx,
                               secure_key_handle_t handle);
key_state_t secure_keystore_get_key_state(keystore_context_t *ctx,
                                         secure_key_handle_t handle);
int secure_keystore_get_key_by_type(keystore_context_t *ctx,
                                   key_type_t type,
                                   secure_key_handle_t *handle);
void secure_keystore_cleanup(keystore_context_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* SECURE_KEYSTORE_H */

