#include "secure_keystore.h"
#include <string.h>
#include <time.h>
#include "crypto_utils.h"

#if USE_HARDWARE_SECURITY
#if USE_TRUSTZONE
#include <arm_trustzone.h>
#elif USE_TPM
#include <tss2/tss2_sys.h>
#elif USE_STM32_SECURE
#include "stm32_secure_hal.h"
#endif
#endif

int secure_keystore_init(keystore_context_t *ctx) {
    if (!ctx) {
        return -1;
    }
   
    memset(ctx, 0, sizeof(keystore_context_t));
    ctx->next_key_id = 1;
   
 #if USE_HARDWARE_SECURITY
#if USE_TRUSTZONE
    if (!secure_world_check()) {
        return -1;
    }
    if (secure_keystore_hw_init() != 0) {
        return -2;
    }
#elif USE_TPM
    TSS2_RC rc = Tss2_Sys_Init(NULL);
    if (rc != TSS2_RC_SUCCESS) {
        return -2;
    }
#elif USE_STM32_SECURE
    if (HAL_SEC_Keystore_Init() != HAL_OK) {
        return -2;
    }
#endif
#endif
   
    ctx->initialized = 1;
    return 0;
}

int secure_keystore_provision_key(keystore_context_t *ctx,
                                 key_type_t type,
                                 secure_key_handle_t *handle) {
    if (!ctx || !handle || !ctx->initialized) {
        return -1;
    }
   
    /* Check if key already exists */
    for (int i = 0; i < MAX_KEYS; i++) {
        if (ctx->handles[i].type == type &&
            ctx->handles[i].state != KEY_STATE_DESTROYED) {
            *handle = ctx->handles[i];
            return 0;
        }
    }
   
    /* Find empty slot */
    int slot = -1;
    for (int i = 0; i < MAX_KEYS; i++) {
        if (ctx->handles[i].state == KEY_STATE_DESTROYED) {
            slot = i;
            break;
        }
    }
   
    if (slot == -1) {
        return -2; /* No available slots */
    }
   
    /* Generate key in hardware */
    uint32_t key_id = ctx->next_key_id++;
   
 #if USE_HARDWARE_SECURITY
#if USE_TRUSTZONE
    key_id = secure_crypto_generate_key(type);
    if (key_id == 0xFFFFFFFF) {
        return -3;
    }
#elif USE_TPM
    /* TPM key generation logic */
    /* Simplified for demo */
    key_id = secure_tpm_generate_key(type);
    if (key_id == 0) {
        return -3;
    }
#elif USE_STM32_SECURE
    key_id = HAL_SEC_GenerateKey(type);
    if (key_id == 0) {
        return -3;
    }
#endif
#endif
   
    handle->key_id = key_id;
    handle->type = type;
    handle->state = KEY_STATE_ACTIVE;
    handle->created_timestamp = (uint64_t)time(NULL);
    handle->expires_timestamp = handle->created_timestamp +
                               (KEY_ROTATION_DAYS * 24 * 3600);
    handle->usage_count = 0;
    handle->rotation_count = 0;
   
    /* Store in context */
    ctx->handles[slot] = *handle;
   
    return 0;
}

int secure_keystore_use_key(keystore_context_t *ctx,
                           secure_key_handle_t handle,
                           uint8_t *operation_buffer,
                           size_t buffer_len,
                           uint8_t *result,
                           size_t *result_len) {
    if (!ctx || !handle.key_id || !operation_buffer || !result || !result_len) {
        return -1;
    }
   
    /* Verify key state */
    if (handle.state != KEY_STATE_ACTIVE) {
        return -2;
    }
   
    /* Check expiration */
    uint64_t current_time = (uint64_t)time(NULL);
    if (current_time > handle.expires_timestamp) {
        return -3; /* Key expired */
    }
   
    /* Increment usage counter */
    handle.usage_count++;
   
 #if USE_HARDWARE_SECURITY
#if USE_TRUSTZONE
    return secure_crypto_use_key(handle.key_id, operation_buffer,
                                buffer_len, result, result_len);
#elif USE_TPM
    return secure_tpm_use_key(handle.key_id, operation_buffer,
                             buffer_len, result, result_len);
#elif USE_STM32_SECURE
    return HAL_SEC_UseKey(handle.key_id, operation_buffer,
                         buffer_len, result, result_len);
#endif
#else
    /* Software fallback - NOT SECURE */
    memset(result, 0x55, *result_len);
    return 0;
#endif
}

int secure_keystore_rotate_key(keystore_context_t *ctx,
                              secure_key_handle_t old_handle,
                              secure_key_handle_t *new_handle) {
    if (!ctx || !old_handle.key_id || !new_handle) {
        return -1;
    }
   
    /* Revoke old key */
    if (secure_keystore_revoke_key(ctx, old_handle) != 0) {
        return -2;
    }
   
    /* Generate new key */
    if (secure_keystore_provision_key(ctx, old_handle.type, new_handle) != 0) {
        return -3;
    }
   
    /* Update rotation count */
    new_handle->rotation_count = old_handle.rotation_count + 1;
   
    return 0;
}

int secure_keystore_revoke_key(keystore_context_t *ctx,
                              secure_key_handle_t handle) {
    if (!ctx || !handle.key_id) {
        return -1;
    }
   
    /* Find key in context */
    for (int i = 0; i < MAX_KEYS; i++) {
        if (ctx->handles[i].key_id == handle.key_id) {
            ctx->handles[i].state = KEY_STATE_REVOKED;
            return 0;
        }
    }
   
    return -2; /* Key not found */
}

int secure_keystore_destroy_key(keystore_context_t *ctx,
                               secure_key_handle_t handle) {
    if (!ctx || !handle.key_id) {
        return -1;
    }
   
    /* Find key in context */
    for (int i = 0; i < MAX_KEYS; i++) {
        if (ctx->handles[i].key_id == handle.key_id) {
            /* Zeroize any references */
            memset(ctx->handles[i].key_material_ref, 0,
                  sizeof(ctx->handles[i].key_material_ref));
            ctx->handles[i].state = KEY_STATE_DESTROYED;
            return 0;
        }
    }
   
    return -2; /* Key not found */
}

key_state_t secure_keystore_get_key_state(keystore_context_t *ctx,
                                         secure_key_handle_t handle) {
    if (!ctx || !handle.key_id) {
        return KEY_STATE_DESTROYED;
    }
   
    for (int i = 0; i < MAX_KEYS; i++) {
        if (ctx->handles[i].key_id == handle.key_id) {
            return ctx->handles[i].state;
        }
    }
   
    return KEY_STATE_DESTROYED;
}

int secure_keystore_get_key_by_type(keystore_context_t *ctx,
                                   key_type_t type,
                                   secure_key_handle_t *handle) {
    if (!ctx || !handle) {
        return -1;
    }
   
    for (int i = 0; i < MAX_KEYS; i++) {
        if (ctx->handles[i].type == type &&
            ctx->handles[i].state == KEY_STATE_ACTIVE) {
            *handle = ctx->handles[i];
            return 0;
        }
    }
   
    return -2; /* Key not found */
}

void secure_keystore_cleanup(keystore_context_t *ctx) {
    if (!ctx) {
        return;
    }
   
    /* Destroy all keys */
    for (int i = 0; i < MAX_KEYS; i++) {
        if (ctx->handles[i].key_id != 0) {
            secure_keystore_destroy_key(ctx, ctx->handles[i]);
        }
    }
   
    memset(ctx, 0, sizeof(keystore_context_t));
}

