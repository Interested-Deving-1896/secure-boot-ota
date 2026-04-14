
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


#ifndef ROLLBACK_PROTECTION_H
#define ROLLBACK_PROTECTION_H

#include <stdint.h>
#include <stddef.h>
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t version_counter;
    uint8_t signature[64];
    uint64_t last_updated;
    uint8_t flags;
    uint8_t reserved[3];
} rollback_protection_t;

int rollback_init(rollback_protection_t *rb);
int rollback_check_and_increment(rollback_protection_t *rb,
                                uint32_t min_version);
int rollback_get_current_version(rollback_protection_t *rb,
                                uint32_t *version);
int rollback_verify_signature(rollback_protection_t *rb);
int rollback_sign(rollback_protection_t *rb);
void rollback_cleanup(rollback_protection_t *rb);

#ifdef __cplusplus
}
#endif

#endif /* ROLLBACK_PROTECTION_H */

