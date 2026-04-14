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



#include "crypto.h"
#include <string.h>

/*
 * NOTE:
 * This is NOT real crypto.
 * Replace with mbedTLS or hardware crypto engine.
 */

int crypto_init(void)
{
    return 0;
}

int crypto_hash(const uint8_t *data, size_t len, uint8_t *out)
{
    if (!data || !out) return -1;

    memset(out, 0, HASH_SIZE);

    for (size_t i = 0; i < len; i++) {
        out[i % HASH_SIZE] ^= data[i];
    }

    return 0;
}

int crypto_verify(const uint8_t *hash, const uint8_t *sig)
{
    // fake check
    return (hash && sig);
}


    
