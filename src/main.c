#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "secure_boot.h"
#include "ota_manager.h"
#include "crypto_utils.h"
#include "config.h"

#define LOG_INFO(fmt, ...) printf("[INFO] " fmt "\n", ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) printf("[ERROR] " fmt "\n", ##__VA_ARGS__)

static void print_banner(void) {
    printf("============================================================\n");
    printf("       SECURE BOOT + OTA UPDATE DEMO (C Implementation)\n");
    printf("============================================================\n");
}

static void print_boot_status(const secure_boot_ctx_t *ctx) {
    boot_status_t status = secure_boot_get_status(ctx);
    const char *status_str = (status == BOOT_STATUS_SECURE) ? "SECURE" : "COMPROMISED";
    printf("\nBoot Status: %s\n", status_str);
}

static void print_ota_status(const ota_manager_ctx_t *ctx) {
    ota_state_t state = ota_get_state(ctx);
    const char *state_str[] = {
        "IDLE", "CHECKING", "DOWNLOADING", "VERIFYING",
        "INSTALLING", "PENDING_REBOOT", "FAILED"
    };
    printf("OTA State: %s\n", state_str[state]);
}

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
   
    print_banner();
   
    /* Initialize cryptographic subsystem */
    if (crypto_init() != CRYPTO_OK) {
        LOG_ERROR("Failed to initialize crypto subsystem");
        return EXIT_FAILURE;
    }
    LOG_INFO("Cryptographic subsystem initialized");
   
    /* Initialize secure boot */
    secure_boot_ctx_t secure_boot_ctx;
    if (secure_boot_init(&secure_boot_ctx) != 0) {
        LOG_ERROR("Failed to initialize secure boot");
        return EXIT_FAILURE;
    }
    LOG_INFO("Secure boot initialized");
   
    /* Initialize OTA manager */
    ota_manager_ctx_t ota_ctx;
    if (ota_manager_init(&ota_ctx, &secure_boot_ctx) != 0) {
        LOG_ERROR("Failed to initialize OTA manager");
        return EXIT_FAILURE;
    }
    LOG_INFO("OTA manager initialized");
   
    /* Phase 1: Secure Boot Sequence */
    printf("\n[PHASE 1] SECURE BOOT SEQUENCE\n");
    printf("----------------------------------------\n");
   
    /* Simulate bootloader verification */
    const uint8_t bootloader_data[] = "mock_bootloader_binary_data";
    int bootloader_ok = secure_boot_verify_bootloader(&secure_boot_ctx,
                                                       bootloader_data,
                                                       sizeof(bootloader_data));
    printf("Bootloader verification: %s\n", bootloader_ok ? "PASS" : "FAIL");
   
    /* Simulate kernel verification */
    const uint8_t kernel_data[] = "mock_kernel_binary_data";
    uint8_t kernel_signature[SIGNATURE_SIZE];
    crypto_sign(kernel_data, sizeof(kernel_data), kernel_signature);
    int kernel_ok = secure_boot_verify_kernel(&secure_boot_ctx,
                                               kernel_data,
                                               sizeof(kernel_data),
                                               kernel_signature);
    printf("Kernel verification: %s\n", kernel_ok ? "PASS" : "FAIL");
   
    print_boot_status(&secure_boot_ctx);
   
    /* Phase 2: OTA Update Check */
    printf("\n[PHASE 2] OTA UPDATE CHECK\n");
    printf("----------------------------------------\n");
   
    ota_update_info_t update_info;
    if (ota_check_for_updates(&ota_ctx, &update_info) == 0 && update_info.available) {
        LOG_INFO("New version available: %s", update_info.version);
        LOG_INFO("Size: %.2f KB", update_info.size_bytes / 1024.0);
       
        /* Phase 3: Download and Verify */
        printf("\n[PHASE 3] DOWNLOAD & VERIFY\n");
        printf("----------------------------------------\n");
       
        uint8_t *firmware_data = NULL;
        size_t firmware_len = 0;
       
        if (ota_download_firmware(&ota_ctx, update_info.download_url,
                                   &firmware_data, &firmware_len) == 0) {
            LOG_INFO("Firmware downloaded: %zu bytes", firmware_len);
           
            firmware_manifest_t manifest;
            if (ota_download_manifest(&ota_ctx, update_info.manifest_url, &manifest) == 0) {
                LOG_INFO("Manifest retrieved: v%s", manifest.version);
               
                if (ota_verify_update(&ota_ctx, firmware_data, firmware_len, &manifest)) {
                    LOG_INFO("Update verified successfully");
                   
                    /* Phase 4: Install */
                    printf("\n[PHASE 4] INSTALL UPDATE\n");
                    printf("----------------------------------------\n");
                   
                    if (ota_install_update(&ota_ctx, firmware_data, firmware_len) == 0) {
                        LOG_INFO("Update installed, pending reboot");
                        LOG_INFO("Current version: %s", ota_ctx.current_version);
                        LOG_INFO("Target version: %s", manifest.version);
                    } else {
                        LOG_ERROR("Installation failed, rolling back...");
                        ota_rollback(&ota_ctx);
                    }
                } else {
                    LOG_ERROR("Update verification failed");
                }
            } else {
                LOG_ERROR("Failed to download manifest");
            }
           
            free(firmware_data);
        } else {
            LOG_ERROR("Download failed");
        }
    } else {
        LOG_INFO("No updates available");
    }
   
    /* Final Status */
    printf("\n============================================================\n");
    printf("FINAL STATUS\n");
    printf("============================================================\n");
    print_boot_status(&secure_boot_ctx);
    print_ota_status(&ota_ctx);
    printf("============================================================\n");
   
    return EXIT_SUCCESS;
}

