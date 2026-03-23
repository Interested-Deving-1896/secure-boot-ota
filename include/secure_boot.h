
#ifndef SECURE_BOOT_H
#define SECURE_BOOT_H

#include <stdint.h>
#include <stddef.h>
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Secure boot verification status
 */
typedef enum {
    BOOT_STATUS_UNKNOWN = 0,
    BOOT_STATUS_SECURE = 1,
    BOOT_STATUS_COMPROMISED = 2
} boot_status_t;

/**
 * @brief Boot event log entry
 */
typedef struct {
    char event_type[32];
    int success;
    char error_msg[64];
    uint64_t timestamp;
} boot_event_t;

/**
 * @brief Secure boot context
 */
typedef struct {
    boot_event_t events[16];
    size_t event_count;
    boot_status_t status;
    uint8_t trusted_keys[KEY_SIZE];
} secure_boot_ctx_t;

/**
 * @brief Initialize secure boot subsystem
 * @param ctx Pointer to secure boot context
 * @return 0 on success
 */
int secure_boot_init(secure_boot_ctx_t *ctx);

/**
 * @brief Verify bootloader integrity
 * @param ctx Secure boot context
 * @param bootloader_data Bootloader binary
 * @param bootloader_len Length of bootloader
 * @return 1 if verified, 0 if failed
 */
int secure_boot_verify_bootloader(secure_boot_ctx_t *ctx,
                                  const uint8_t *bootloader_data,
                                  size_t bootloader_len);

/**
 * @brief Verify kernel signature
 * @param ctx Secure boot context
 * @param kernel_data Kernel binary
 * @param kernel_len Length of kernel
 * @param signature Kernel signature
 * @return 1 if verified, 0 if failed
 */
int secure_boot_verify_kernel(secure_boot_ctx_t *ctx,
                              const uint8_t *kernel_data,
                              size_t kernel_len,
                              const uint8_t *signature);

/**
 * @brief Get current boot status
 * @param ctx Secure boot context
 * @return Current boot status
 */
boot_status_t secure_boot_get_status(const secure_boot_ctx_t *ctx);

/**
 * @brief Get boot event log
 * @param ctx Secure boot context
 * @param events Output array for events
 * @param max_events Maximum events to copy
 * @return Number of events copied
 */
size_t secure_boot_get_events(const secure_boot_ctx_t *ctx,
                              boot_event_t *events,
                              size_t max_events);

/**
 * @brief Log boot event
 * @param ctx Secure boot context
 * @param event_type Event type string
 * @param success Whether event succeeded
 * @param error_msg Error message if failed
 */
void secure_boot_log_event(secure_boot_ctx_t *ctx,
                           const char *event_type,
                           int success,
                           const char *error_msg);

#ifdef __cplusplus
}
#endif

#endif /* SECURE_BOOT_H */
