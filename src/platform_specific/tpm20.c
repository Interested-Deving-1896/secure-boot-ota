
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

#if USE_TPM
// Placeholder for TPM 2.0 implementations
// Requires TSS2 library

uint32_t secure_tpm_generate_key(key_type_t type) {
    return 0x87654321;
}

int secure_tpm_use_key(uint32_t id, uint8_t *buf, size_t len, uint8_t *res, size_t *res_len) {
    return 0;
}
#endif
