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


#ifndef OTA_MANAGER_H
#define OTA_MANAGER_H

#include <stdint.h>
#include <stddef.h>
#include "config.h"
#include "secure_boot.h"
#include "crypto_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief OTA update state machine states
 */
typedef enum {
    OTA_STATE_IDLE = 0,
    OTA_STATE_CHECKING = 1,
    OTA_STATE_DOWNLOADING = 2,
    OTA_STATE_VERIFYING = 3,
    OTA_STATE_INSTALLING = 4,
    OTA_STATE_PENDING_REBOOT = 5,
    OTA_STATE_FAILED = 6
} ota_state_t;

/**
 * @brief OTA update information
 */
typedef struct {
    int available;
    char version[MAX_VERSION_LEN];
    char download_url[MAX_PATH_LEN];
    char manifest_url[MAX_PATH_LEN];
    size_t size_bytes;
    char release_notes[256];
} ota_update_info_t;

/**
 * @brief OTA update context
 */
typedef struct {
    secure_boot_ctx_t *secure_boot;
    char current_version[MAX_VERSION_LEN];
    uint8_t *backup_partition;
    size_t backup_size;
    ota_state_t state;
    int rollback_counter;
    uint8_t active_partition;
} ota_manager_ctx_t;

/**
 * @brief Initialize OTA manager
 * @param ctx OTA manager context
 * @param secure_boot Secure boot context pointer
 * @return 0 on success
 */
int ota_manager_init(ota_manager_ctx_t *ctx, secure_boot_ctx_t *secure_boot);

/**
 * @brief Check for available updates
 * @param ctx OTA manager context
 * @param update_info Output update information
 * @return 0 if update available, -1 otherwise
 */
int ota_check_for_updates(ota_manager_ctx_t *ctx, ota_update_info_t *update_info);

/**
 * @brief Download firmware binary
 * @param ctx OTA manager context
 * @param url Download URL
 * @param firmware_data Output firmware buffer
 * @param firmware_len Output firmware length
 * @return 0 on success
 */
int ota_download_firmware(ota_manager_ctx_t *ctx,
                          const char *url,
                          uint8_t **firmware_data,
                          size_t *firmware_len);

/**
 * @brief Download and parse manifest
 * @param ctx OTA manager context
 * @param url Manifest URL
 * @param manifest Output manifest structure
 * @return 0 on success
 */
int ota_download_manifest(ota_manager_ctx_t *ctx,
                          const char *url,
                          firmware_manifest_t *manifest);

/**
 * @brief Verify firmware update
 * @param ctx OTA manager context
 * @param firmware_data Firmware binary
 * @param firmware_len Length of firmware
 * @param manifest Update manifest
 * @return 1 if verified, 0 if failed
 */
int ota_verify_update(ota_manager_ctx_t *ctx,
                      const uint8_t *firmware_data,
                      size_t firmware_len,
                      const firmware_manifest_t *manifest);

/**
 * @brief Install firmware update
 * @param ctx OTA manager context
 * @param firmware_data Firmware binary
 * @param firmware_len Length of firmware
 * @return 0 on success
 */
int ota_install_update(ota_manager_ctx_t *ctx,
                       const uint8_t *firmware_data,
                       size_t firmware_len);

/**
 * @brief Rollback to previous firmware
 * @param ctx OTA manager context
 * @return 0 on success
 */
int ota_rollback(ota_manager_ctx_t *ctx);

/**
 * @brief Compare version strings
 * @param v1 First version
 * @param v2 Second version
 * @return >0 if v1 > v2, <0 if v1 < v2, 0 if equal
 */
int ota_compare_versions(const char *v1, const char *v2);

/**
 * @brief Get current OTA state
 * @param ctx OTA manager context
 * @return Current OTA state
 */
ota_state_t ota_get_state(const ota_manager_ctx_t *ctx);

/**
 * @brief Log OTA event
 * @param ctx OTA manager context
 * @param event Event type
 * @param success Whether event succeeded
 * @param message Event message
 */
void ota_log_event(ota_manager_ctx_t *ctx,
                   const char *event,
                   int success,
                   const char *message);

#ifdef __cplusplus
}
#endif

#endif /* OTA_MANAGER_H */

