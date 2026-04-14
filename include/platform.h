
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


#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

/*
 * Platform abstraction (very thin)
 * In real firmware this wraps HAL / BSP
 */

void platform_log(const char *fmt, ...);

// TODO: replace with actual flash driver
int flash_write(uint32_t addr, const uint8_t *data, uint32_t len);
int flash_read(uint32_t addr, uint8_t *data, uint32_t len);

#endif
