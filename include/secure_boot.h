
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


#ifndef SECURE_BOOT_H
#define SECURE_BOOT_H

#include <stdint.h>

typedef enum {
    BOOT_UNKNOWN = 0,
    BOOT_OK,
    BOOT_FAIL
} boot_state_t;

typedef struct {
    boot_state_t state;
} secure_boot_ctx_t;

int secure_boot_init(secure_boot_ctx_t *ctx);
int secure_boot_verify(secure_boot_ctx_t *ctx,
                       const uint8_t *image,
                       uint32_t len);

#endif
