
#ifndef OTA_HANDLER_H
#define OTA_HANDLER_H

#include <stdint.h>
#include <stdbool.h>

/* OTA States */
typedef enum {
    OTA_STATE_IDLE = 0,
    OTA_STATE_DOWNLOADING,
    OTA_STATE_VALIDATING,
    OTA_STATE_SWITCHING,
    OTA_STATE_ROLLBACK,
    OTA_STATE_FAILED
} ota_state_t;

/* OTA Event Callbacks */
typedef void (*ota_progress_cb)(uint32_t downloaded, uint32_t total);
typedef void (*ota_status_cb)(ota_state_t state, int error_code);

/* OTA Configuration */
typedef struct {
    uint32_t max_download_size;
    uint32_t timeout_ms;
    uint32_t retry_count;
    bool rollback_enabled;
    uint32_t rollback_threshold;  /* Failed boots before rollback */
} ota_config_t;

/* Initialize OTA subsystem */
int ota_init(const ota_config_t *config);

/* Start OTA update process */
int ota_start_update(const char *firmware_url,
                      const uint8_t *expected_signature,
                      size_t signature_len);

/* Download and validate firmware */
int ota_download_firmware(const char *url,
                          uint8_t *buffer,
                          size_t buffer_size);

/* Verify firmware signature before switching */
int ota_verify_firmware(const uint8_t *firmware,
                        size_t size,
                        const uint8_t *signature,
                        size_t signature_len);

/* Switch to new firmware (triggers reboot) */
int ota_switch_firmware(void);

/* Rollback to previous firmware */
int ota_rollback(void);

/* Get current OTA state */
ota_state_t ota_get_state(void);

/* Register callbacks */
void ota_register_progress_callback(ota_progress_cb cb);
void ota_register_status_callback(ota_status_cb cb);

/* Check boot success counter (for rollback logic) */
int ota_check_boot_success(void);
void ota_increment_boot_counter(void);
void ota_reset_boot_counter(void);

#endif /* OTA_HANDLER_H */
