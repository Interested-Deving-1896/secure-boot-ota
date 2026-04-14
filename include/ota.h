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


##ifndef OTA_H
#define OTA_H

#include <stdint.h>
#include "secure_boot.h"

typedef enum {
    OTA_IDLE = 0,
    OTA_DOWNLOADING,
    OTA_VERIFYING,
    OTA_DONE,
    OTA_ERROR
} ota_state_t;

typedef struct {
    ota_state_t state;
    char current_version[16];
} ota_ctx_t;

int ota_init(ota_ctx_t *ctx, secure_boot_ctx_t *sb);
int ota_check(ota_ctx_t *ctx);
int ota_perform_update(ota_ctx_t *ctx);

#endif

