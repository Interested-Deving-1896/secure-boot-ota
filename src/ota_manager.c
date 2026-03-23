
#include "ota_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int ota_manager_init(ota_manager_ctx_t *ctx, secure_boot_ctx_t *secure_boot) {
    if (!ctx || !secure_boot) {
        return -1;
    }
   
    memset(ctx, 0, sizeof(ota_manager_ctx_t));
    ctx->secure_boot = secure_boot;
    strncpy(ctx->current_version, FIRMWARE_VERSION, MAX_VERSION_LEN - 1);
    ctx->state = OTA_STATE_IDLE;
    ctx->active_partition = PARTITION_ACTIVE;
   
    return 0;
}

int ota_check_for_updates(ota_manager_ctx_t *ctx, ota_update_info_t *update_info) {
    if (!ctx || !update_info) {
        return -1;
    }
   
    memset(update_info, 0, sizeof(ota_update_info_t));
   
    /* Mock update check */
    update_info->available = 1;
    strncpy(update_info->version, "1.1.0", MAX_VERSION_LEN - 1);
    snprintf(update_info->download_url, MAX_PATH_LEN, "%s/firmware/v1.1.0.bin",
             OTA_SERVER_URL);
    snprintf(update_info->manifest_url, MAX_PATH_LEN, "%s/manifest/v1.1.0.json",
             OTA_SERVER_URL);
    update_info->size_bytes = 1048576;
    strncpy(update_info->release_notes, "Security patches and bug fixes",
            sizeof(update_info->release_notes) - 1);
   
    ota_log_event(ctx, "UPDATE_CHECK", 1, "Update available");
    return 0;
}

int ota_download_firmware(ota_manager_ctx_t *ctx,
                          const char *url,
                          uint8_t **firmware_data,
                          size_t *firmware_len) {
    if (!ctx || !url || !firmware_data || !firmware_len) {
        return -1;
    }
   
    /* Mock download */
    *firmware_len = 1024 * 1024;  /* 1 MB */
    *firmware_data = malloc(*firmware_len);
   
    if (!*firmware_data) {
        ota_log_event(ctx, "DOWNLOAD", 0, "Memory allocation failed");
        return -1;
    }
   
    memset(*firmware_data, 0x55, *firmware_len);
    ota_log_event(ctx, "DOWNLOAD", 1, "Downloaded 1048576 bytes");
    return 0;
}

int ota_download_manifest(ota_manager_ctx_t *ctx,
                          const char *url,
                          firmware_manifest_t *manifest) {
    if (!ctx || !url || !manifest) {
        return -1;
    }
   
    memset(manifest, 0, sizeof(firmware_manifest_t));
    strncpy(manifest->version, "1.1.0", MAX_VERSION_LEN - 1);
    manifest->timestamp = (uint64_t)time(NULL);
    strncpy(manifest->device_id, DEVICE_ID, MAX_VERSION_LEN - 1);
   
    /* Mock manifest data */
    memset(manifest->firmware_hash, 0x55, HASH_SIZE);
    memset(manifest->signature, 0x66, SIGNATURE_SIZE);
   
    ota_log_event(ctx, "MANIFEST_DOWNLOAD", 1, "Manifest retrieved");
    return 0;
}

int ota_verify_update(ota_manager_ctx_t *ctx,
                      const uint8_t *firmware_data,
                      size_t firmware_len,
                      const firmware_manifest_t *manifest) {
    if (!ctx || !firmware_data || !manifest || firmware_len == 0) {
        return 0;
    }
   
    /* Verify hash */
    uint8_t calculated_hash[HASH_SIZE];
    crypto_hash(firmware_data, firmware_len, calculated_hash);
   
    if (memcmp(calculated_hash, manifest->firmware_hash, HASH_SIZE) != 0) {
        ota_log_event(ctx, "VERIFICATION", 0, "Hash mismatch");
        return 0;
    }
   
    /* Verify signature */
    if (!crypto_verify(manifest->firmware_hash, HASH_SIZE, manifest->signature)) {
        ota_log_event(ctx, "VERIFICATION", 0, "Signature invalid");
        return 0;
    }
   
    /* Verify version is newer */
    if (ota_compare_versions(manifest->version, ctx->current_version) <= 0) {
        ota_log_event(ctx, "VERIFICATION", 0, "Version not newer");
        return 0;
    }
   
    ota_log_event(ctx, "VERIFICATION", 1, "Update verified successfully");
    return 1;
}

int ota_install_update(ota_manager_ctx_t *ctx,
                       const uint8_t *firmware_data,
                       size_t firmware_len) {
    if (!ctx || !firmware_data || firmware_len == 0) {
        return -1;
    }
   
    ctx->state = OTA_STATE_INSTALLING;
   
    /* Create backup */
    if (ctx->backup_partition) {
        free(ctx->backup_partition);
    }
    ctx->backup_size = firmware_len;
    ctx->backup_partition = malloc(firmware_len);
   
    if (!ctx->backup_partition) {
        ota_log_event(ctx, "INSTALL", 0, "Backup allocation failed");
        return -1;
    }
   
    memcpy(ctx->backup_partition, firmware_data, firmware_len);
   
    /* Mark for boot on next restart */
    ctx->active_partition = PARTITION_INACTIVE;
    ctx->rollback_counter++;
   
    ctx->state = OTA_STATE_PENDING_REBOOT;
    ota_log_event(ctx, "INSTALL", 1, "Update ready for reboot");
    return 0;
}

int ota_rollback(ota_manager_ctx_t *ctx) {
    if (!ctx || !ctx->backup_partition) {
        return -1;
    }
   
    /* Restore from backup */
    ctx->active_partition = PARTITION_ACTIVE;
    free(ctx->backup_partition);
    ctx->backup_partition = NULL;
   
    ota_log_event(ctx, "ROLLBACK", 1, "Rolled back successfully");
    return 0;
}

int ota_compare_versions(const char *v1, const char *v2) {
    if (!v1 || !v2) {
        return 0;
    }
   
    int v1_major = 0, v1_minor = 0, v1_patch = 0;
    int v2_major = 0, v2_minor = 0, v2_patch = 0;
   
    sscanf(v1, "%d.%d.%d", &v1_major, &v1_minor, &v1_patch);
    sscanf(v2, "%d.%d.%d", &v2_major, &v2_minor, &v2_patch);
   
    if (v1_major != v2_major) {
        return v1_major - v2_major;
    }
    if (v1_minor != v2_minor) {
        return v1_minor - v2_minor;
    }
    return v1_patch - v2_patch;
}

ota_state_t ota_get_state(const ota_manager_ctx_t *ctx) {
    if (!ctx) {
        return OTA_STATE_IDLE;
    }
    return ctx->state;
}

void ota_log_event(ota_manager_ctx_t *ctx,
                   const char *event,
                   int success,
                   const char *message) {
    if (!ctx || !event) {
        return;
    }
   
    printf("[%ld] %s: %s - %s\n",
           (long)time(NULL),
           event,
           success ? "SUCCESS" : "FAILED",
           message ? message : "N/A");
}
