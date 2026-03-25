
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "crypto_utils.h"
#include "secure_keystore.h"
#include "secure_boot.h"
#include "rollback_protection.h"
#include "ota_manager.h"

int main(void) {
    printf("=== Secure Boot & OTA System Initialization ===\n");

    // 1. Initialize Crypto Subsystem
    if (crypto_init() != CRYPTO_OK) {
        printf("ERROR: Crypto initialization failed.\n");
        return 1;
    }
    printf("[OK] Crypto subsystem initialized.\n");

    // 2. Initialize Secure Keystore
    keystore_context_t keystore;
    if (secure_keystore_init(&keystore) != 0) {
        printf("ERROR: Keystore initialization failed.\n");
        crypto_cleanup();
        return 1;
    }
    printf("[OK] Secure Keystore initialized.\n");

    // Provision Root of Trust (Simulated)
    secure_key_handle_t rot_key;
    if (secure_keystore_provision_key(&keystore, KEY_TYPE_ROOT_OF_TRUST, &rot_key) != 0) {
        printf("WARNING: Could not provision ROT key (expected in simulation).\n");
    }

    // 3. Initialize Rollback Protection
    rollback_protection_t rb;
    if (rollback_init(&rb) != 0) {
        printf("ERROR: Rollback protection init failed.\n");
        secure_keystore_cleanup(&keystore);
        crypto_cleanup();
        return 1;
    }
    printf("[OK] Rollback protection initialized.\n");

    // 4. Initialize Secure Boot Context
    secure_boot_ctx_t boot_ctx;
    if (secure_boot_init(&boot_ctx, &keystore) != 0) {
        printf("ERROR: Secure boot init failed.\n");
        rollback_cleanup(&rb);
        secure_keystore_cleanup(&keystore);
        crypto_cleanup();
        return 1;
    }
    printf("[OK] Secure boot context initialized.\n");

    // 5. Simulate Boot Process
    // In a real device, bootloader_data and kernel_data would be loaded from flash
    const uint8_t dummy_bootloader[] = {0x01, 0x02, 0x03};
    const uint8_t dummy_kernel[] = {0xAA, 0xBB, 0xCC};
    uint8_t dummy_signature[SIGNATURE_SIZE] = {0}; // In real life, this is verified

    printf("\n--- Verifying Boot Components ---\n");
    
    if (secure_boot_verify_bootloader(&boot_ctx, dummy_bootloader, sizeof(dummy_bootloader)) != 0) {
        printf("CRITICAL: Bootloader verification failed!\n");
        // Halt or enter recovery mode
    }

    if (secure_boot_verify_kernel(&boot_ctx, dummy_kernel, sizeof(dummy_kernel), dummy_signature) != 0) {
        printf("CRITICAL: Kernel verification failed!\n");
        // Halt or enter recovery mode
    }

    // 6. Check Rollback Version
    uint32_t current_ver = 0;
    rollback_get_current_version(&rb, &current_ver);
    printf("Current Firmware Version Counter: %u\n", current_ver);

    // 7. Initialize OTA Manager
    ota_manager_t ota_ctx;
    if (ota_init(&ota_ctx) != 0) {
        printf("WARNING: OTA manager init failed (network unavailable?).\n");
    } else {
        printf("[OK] OTA Manager initialized.\n");
        // Check for updates logic would go here
    }

    // Cleanup
    printf("\n--- System Shutdown ---\n");
    secure_boot_cleanup(&boot_ctx);
    rollback_cleanup(&rb);
    secure_keystore_cleanup(&keystore);
    crypto_cleanup();

    printf("System shutdown complete.\n");
    return 0;
}
