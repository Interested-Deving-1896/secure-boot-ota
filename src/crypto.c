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



#include "ota_manager.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "config.h"
#include "crypto_utils.h"
#include "secure_boot.h"
#include "rollback_protection.h"

static int ota_initialized = 0;

int ota_init(ota_manager_t *ctx) {
    if (!ctx) return -1;
    memset(ctx, 0, sizeof(ota_manager_t));
    ctx->state = OTA_STATE_IDLE;
    ctx->last_check = 0;
    ota_initialized = 1;
    return 0;
}

int ota_check_for_update(ota_manager_t *ctx) {
    if (!ctx || !ota_initialized) return -1;
    
    uint64_t now = (uint64_t)time(NULL);
    if (now - ctx->last_check < UPDATE_CHECK_INTERVAL) {
        return 0; // Too soon
    }

    ctx->state = OTA_STATE_CHECKING;
    printf("Checking for updates at %s...\n", OTA_SERVER_URL);
    
    // Simulate network request
    // In production: HTTP GET to OTA_SERVER_URL/manifest.json
    // Parse response, compare version_number with current
    
    // Simulating "Update Available"
    ctx->pending_update.version = 101;
    ctx->pending_update.size = 1024 * 1024; // 1MB
    ctx->state = OTA_STATE_UPDATE_AVAILABLE;
    
    ctx->last_check = now;
    return 1; // Update available
}

int ota_download_firmware(ota_manager_t *ctx, const char *url) {
    if (!ctx || ctx->state != OTA_STATE_UPDATE_AVAILABLE) return -1;
    
    ctx->state = OTA_STATE_DOWNLOADING;
    printf("Downloading firmware from %s...\n", url);
    
    // Simulate download progress
    for(int i=0; i<=100; i+=10) {
        // printf("Progress: %d%%\n", i);
    }
    
    ctx->downloaded_size = ctx->pending_update.size;
    ctx->state = OTA_STATE_DOWNLOADED;
    return 0;
}

int ota_verify_and_install(ota_manager_t *ctx, const uint8_t *firmware_data, size_t len) {
    if (!ctx || !firmware_data || len == 0) return -1;
    
    ctx->state = OTA_STATE_VERIFYING;
    
    // 1. Verify Manifest
    firmware_manifest_t manifest;
    // Assume manifest is prepended to firmware_data in real scenario
    // For demo, we create a dummy one
    memset(&manifest, 0, sizeof(manifest));
    
    crypto_result_t res = crypto_verify_manifest(&manifest, firmware_data, len);
    if (res != CRYPTO_OK) {
        printf("Firmware verification failed: %d\n", res);
        ctx->state = OTA_STATE_FAILED;
        return -1;
    }
    
    // 2. Check Rollback Protection
    uint32_t new_version = manifest.version_number;
    uint32_t current_version = 0;
    rollback_protection_t rb; // Should be passed in or global
    // rollback_get_current_version(&rb, &current_version);
    
    if (new_version <= current_version) {
        printf("Rollback attack detected! New version %u <= Current %u\n", new_version, current_version);
        ctx->state = OTA_STATE_FAILED;
        return -2;
    }
    
    // 3. Write to inactive partition
    ctx->state = OTA_STATE_WRITING;
    // flash_write(PARTITION_INACTIVE, firmware_data, len);
    
    // 4. Update Boot Flags
    // set_boot_partition(PARTITION_INACTIVE);
    
    // 5. Increment Rollback Counter
    // rollback_check_and_increment(&rb, new_version);
    
    ctx->state = OTA_STATE_READY_TO_REBOOT;
    printf("Firmware verified and installed. Ready to reboot.\n");
    return 0;
}

void ota_cleanup(ota_manager_t *ctx) {
    if (!ctx) return;
    memset(ctx, 0, sizeof(ota_manager_t));
    ota_initialized = 0;
}
