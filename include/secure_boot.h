
#ifndef SECURE_BOOT_H
#define SECURE_BOOT_H

#include <stdint.h>
#include <stddef.h>
#include "config.h"
#include "secure_keystore.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BOOT_STATUS_UNKNOWN = 0,
    BOOT_STATUS_SECURE = 1,
    BOOT_STATUS_COMPROMISED = 2
} boot_status_t;

typedef struct {
    char event_type[32];
    int success;
    char error_msg[64];
    uint64_t timestamp;
    uint32_t sequence_num;
} boot_event_t;

typedef struct {
    boot_event_t events[16];
    size_t event_count;
    boot_status_t status;
    secure_key_handle_t root_key;
    uint8_t initialized;
} secure_boot_ctx_t;

int secure_boot_init(secure_boot_ctx_t *ctx, keystore_context_t *keystore);
int secure_boot_verify_bootloader(secure_boot_ctx_t *ctx,
                                 const uint8_t *bootloader_data,
                                 size_t bootloader_len);
int secure_boot_verify_kernel(secure_boot_ctx_t *ctx,
                             const uint8_t *kernel_data,
                             size_t kernel_len,
                             const uint8_t *signature);
boot_status_t secure_boot_get_status(const secure_boot_ctx_t *ctx);
size_t secure_boot_get_events(const secure_boot_ctx_t *ctx,
                             boot_event_t *events,
                             size_t max_events);
void secure_boot_log_event(secure_boot_ctx_t *ctx,
                          const char *event_type,
                          int success,
                          const char *error_msg);
void secure_boot_cleanup(secure_boot_ctx_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* SECURE_BOOT_H */
