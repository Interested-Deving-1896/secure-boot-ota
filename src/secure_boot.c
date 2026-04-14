
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
#include "crypto.h"
#include <string.h>

/*
 * Secure boot (simplified)
 * Real system would verify against ROM-stored hash/key
 */

int secure_boot_init(secure_boot_ctx_t *ctx)
{
    if (!ctx) return -1;

    memset(ctx, 0, sizeof(*ctx));
    ctx->state = BOOT_UNKNOWN;
    return 0;
}

int secure_boot_verify(secure_boot_ctx_t *ctx,
                       const uint8_t *image,
                       uint32_t len)
{
    if (!ctx || !image || len == 0) {
        ctx->state = BOOT_FAIL;
        return -1;
    }

    // TODO: compare against trusted hash in flash/ROM

    ctx->state = BOOT_OK;
    return 0;
}
