
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

#if USE_STM32_SECURE
// Placeholder for STM32 Secure HAL

int HAL_SEC_Init(void) { return 0; }
int HAL_SEC_SHA256(const uint8_t *data, size_t len, uint8_t *out) { return 0; }
int HAL_SEC_ECDSA_Sign(const uint8_t *data, size_t len, uint8_t *sig, size_t *len_out) { return 0; }
int HAL_SEC_ECDSA_Verify(const uint8_t *data, size_t len, const uint8_t *sig, size_t len_sig) { return 1; }
int HAL_SEC_Keystore_Init(void) { return 0; }
uint32_t HAL_SEC_GenerateKey(key_type_t type) { return 0xAAAA; }
int HAL_SEC_UseKey(uint32_t id, uint8_t *buf, size_t len, uint8_t *res, size_t *res_len) { return 0; }
int HAL_SEC_Deinit(void) { return 0; }
#endif
