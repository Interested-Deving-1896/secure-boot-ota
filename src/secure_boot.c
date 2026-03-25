
#include "secure_boot.h"
#include <string.h>
#include <time.h>
#include "crypto_utils.h"
#include "rollback_protection.h"

int secure_boot_init(secure_boot_ctx_t *ctx, keystore_context_t *keystore) {
    if (!ctx || !keystore) {
        return -1;
    }
   
    memset(ctx, 0, sizeof(secure_boot_ctx_t));
    ctx->status = BOOT_STATUS_UNKNOWN;
   
    /* Get root of trust key */
    if (secure_keystore_get_key_by_type(keystore, KEY_TYPE_ROOT_OF_TRUST,
                                       &ctx->root_key) != 0) {
        secure_boot_log_event(ctx, "INIT", 0, "Root key not found");
        return -2;
    }
   
    ctx->initialized = 1;
    secure_boot_log_event(ctx, "INIT", 1, "Secure boot initialized");
    return 0;
}

int secure_boot_verify_bootloader(secure_boot_ctx_t *ctx,
                                 const uint8_t *bootloader_data,
                                 size_t bootloader_len) {
    if (!ctx || !bootloader_data || bootloader_len == 0 || !ctx->initialized) {
        secure_boot_log_event(ctx, "BOOTLOADER
