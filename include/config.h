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


#ifndef CONFIG_H
#define CONFIG_H

/* Device Configuration */
#define DEVICE_ID           "device-001"
#define FIRMWARE_VERSION    "1.0.0"
#define MAX_VERSION_LEN     32
#define MAX_PATH_LEN        256
#define MAX_RELEASE_NOTES   512

/* Security Configuration */
#define KEY_SIZE            32
#define HASH_SIZE           32
#define SIGNATURE_SIZE      64
#define MAX_KEYS            16
#define KEY_ROTATION_DAYS   90

/* OTA Configuration */
#define OTA_SERVER_URL      "https://firmware.example.com"
#define UPDATE_CHECK_INTERVAL 86400
#define BOOT_TIMEOUT        30
#define MAX_ROLLBACK_ATTEMPTS 3
#define OTA_RETRY_COUNT     3
#define OTA_TIMEOUT_MS      30000

/* Partition Configuration */
#define PARTITION_ACTIVE    0
#define PARTITION_INACTIVE  1
#define PARTITION_COUNT     2
#define PARTITION_SIZE_MB   64

/* Logging Levels */
#define LOG_LEVEL_ERROR     0
#define LOG_LEVEL_WARN      1
#define LOG_LEVEL_INFO      2
#define LOG_LEVEL_DEBUG     3

/* Default Log Level */
#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_INFO
#endif

/* Memory Safety */
#define MAX_ALLOC_SIZE      (1024 * 1024)  /* 1MB max allocation */
#define STACK_CANARY_VALUE  0xDEADBEEF

/* Hardware Security Flags */
#define USE_HARDWARE_SECURITY 1
#define USE_TPM               0
#define USE_TRUSTZONE         1
#define USE_STM32_SECURE      0

#endif /* CONFIG_H */
