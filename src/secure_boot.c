
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


#include "secure_boot.h"
#include <string.h>
#include <time.h>
#include "crypto_utils.h"
#include "rollback_protection.h"

int secure_boot_init(secure_boot_ctx_t *ctx, keystore_context_t *keystore) {
    if (!ctx || !keystore) {
        return -1;
    }
   
    memset(ctx, 0, sizeof(secure_boot_ctx_t));
    ctx->status = BOOT_STATUS_UNKNOWN;
   
    /* Get root of trust key */
    if (secure_keystore_get_key_by_type(keystore, KEY_TYPE_ROOT_OF_TRUST,
                                       &ctx->root_key) != 0) {
        secure_boot_log_event(ctx, "INIT", 0, "Root key not found");
        return -2;
    }
   
    ctx->initialized = 1;
    secure_boot_log_event(ctx, "INIT", 1, "Secure boot initialized");
    return 0;
}

int secure_boot_verify_bootloader(secure_boot_ctx_t *ctx,
                                 const uint8_t *bootloader_data,
                                 size_t bootloader_len) {
    if (!ctx || !bootloader_data || bootloader_len == 0 || !ctx->initialized) {
        secure_boot_log_event(ctx, "BOOTLOADER
