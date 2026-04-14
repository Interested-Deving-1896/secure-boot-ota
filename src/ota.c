

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


#include "ota.h"
#include "crypto.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
 * OTA logic
 *
 * NOTE:
 * - currently RAM-based (not realistic for big firmware)
 * - should stream directly to flash
 */

static secure_boot_ctx_t *g_sb = NULL;

int ota_init(ota_ctx_t *ctx, secure_boot_ctx_t *sb)
{
    if (!ctx || !sb) return -1;

    memset(ctx, 0, sizeof(*ctx));
    strncpy(ctx->current_version, FW_VERSION, sizeof(ctx->current_version)-1);

    g_sb = sb;
    ctx->state = OTA_IDLE;
    return 0;
}

int ota_check(ota_ctx_t *ctx)
{
    if (!ctx) return -1;

    // Stub: always "update available"
    return 1;
}

int ota_perform_update(ota_ctx_t *ctx)
{
    if (!ctx) return -1;

    ctx->state = OTA_DOWNLOADING;

    size_t fw_len = 256 * 1024; // 256 KB test image

    if (fw_len > MAX_FW_SIZE) {
        ctx->state = OTA_ERROR;
        return -1;
    }

    uint8_t *fw = malloc(fw_len);
    if (!fw) {
        ctx->state = OTA_ERROR;
        return -1;
    }

    memset(fw, 0xAA, fw_len);

    ctx->state = OTA_VERIFYING;

    uint8_t hash[HASH_SIZE];
    crypto_hash(fw, fw_len, hash);

    if (!crypto_verify(hash, hash)) {
        ctx->state = OTA_ERROR;
        free(fw);
        return -1;
    }

    // TODO: write to inactive partition (flash driver missing)

    ctx->state = OTA_DONE;
    free(fw);

    return 0;
}
