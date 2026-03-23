#include "ota_handler.h"
#include "secure_boot.h"
#include "crypto_api.h"
#include "flash_encryption.h"
#include <string.h>
#include <stdlib.h>

/* OTA metadata structure stored in flash */
typedef struct {
    uint32_t magic;              /* 0xDEADBEEF for validation */
    uint32_t firmware_version;
    uint32_t firmware_size;
    uint8_t signature[64];       /* ECDSA P-256 signature */
    uint8_t hash[32];            /* SHA-256 hash of firmware */
    uint32_t boot_attempts;      /* Track failed boots for rollback */
    uint32_t rollback_enabled;
    uint8_t reserved[16];
} ota_metadata_t;

/* OTA state */
static ota_state_t g_ota_state = OTA_STATE_IDLE;
static ota_config_t g_config;
static ota_progress_cb g_progress_cb = NULL;
static ota_status_cb g_status_cb = NULL;

/* Flash partition offsets (adjust for your hardware) */
#define PARTITION_A_OFFSET  0x08000000
#define PARTITION_B_OFFSET  0x08040000
#define METADATA_OFFSET     0x0807F000
#define ACTIVE_PARTITION    0  /* 0 = A, 1 = B */

/* Current state */
static uint8_t g_active_partition = 0;
static uint8_t g_pending_partition = 0;

int ota_init(const ota_config_t *config)
{
    if (config == NULL) {
        return -1;
    }
   
    g_config = *config;
    g_ota_state = OTA_STATE_IDLE;
   
    /* Initialize flash encryption subsystem */
    if (flash_encryption_init() != 0) {
        return -1;
    }
   
    /* Validate existing metadata */
    ota_metadata_t metadata;
    if (read_metadata(&metadata) == 0) {
        if (metadata.magic != 0xDEADBEEF) {
            /* Invalid metadata, initialize fresh */
            init_default_metadata();
        }
    }
   
    return 0;
}

int ota_start_update(const char *firmware_url,
                      const uint8_t *expected_signature,
                      size_t signature_len)
{
    if (firmware_url == NULL || expected_signature == NULL) {
        return -1;
    }
   
    if (signature_len != ECDSA_SIGNATURE_SIZE) {
        return -1;
    }
   
    g_ota_state = OTA_STATE_DOWNLOADING;
    notify_status(OTA_STATE_DOWNLOADING, 0);
   
    /* Allocate download buffer */
    uint8_t *download_buffer = malloc(g_config.max_download_size);
    if (download_buffer == NULL) {
        g_ota_state = OTA_STATE_FAILED;
        notify_status(OTA_STATE_FAILED, -1);
        return -1;
    }
   
    /* Download firmware */
    int download_result = ota_download_firmware(firmware_url,
                                                 download_buffer,
                                                 g_config.max_download_size);
    if (download_result != 0) {
        free(download_buffer);
        g_ota_state = OTA_STATE_FAILED;
        notify_status(OTA_STATE_FAILED, download_result);
        return -1;
    }
   
    /* Validate signature */
    g_ota_state = OTA_STATE_VALIDATING;
    notify_status(OTA_STATE_VALIDATING, 0);
   
    int verify_result = ota_verify_firmware(download_buffer,
                                            g_config.max_download_size,
                                            expected_signature,
                                            signature_len);
    if (verify_result != 0) {
        free(download_buffer);
        g_ota_state = OTA_STATE_FAILED;
        notify_status(OTA_STATE_FAILED, -2);
        return -1;
    }
   
    /* Store pending firmware in inactive partition */
    g_pending_partition = (g_active_partition == 0) ? 1 : 0;
    uint32_t target_offset = (g_pending_partition == 0) ?
                             PARTITION_A_OFFSET : PARTITION_B_OFFSET;
   
    int write_result = write_to_flash(target_offset,
                                       download_buffer,
                                       g_config.max_download_size);
    free(download_buffer);
   
    if (write_result != 0) {
        g_ota_state = OTA_STATE_FAILED;
        notify_status(OTA_STATE_FAILED, -3);
        return -1;
    }
   
    /* Update metadata */
    ota_metadata_t metadata;
    memset(&metadata, 0, sizeof(metadata));
    metadata.magic = 0xDEADBEEF;
    metadata.firmware_version = calculate_firmware_version(download_buffer,
                                                           g_config.max_download_size);
    metadata.firmware_size = g_config.max_download_size;
    memcpy(metadata.signature, expected_signature, signature_len);
    crypto_sha256(download_buffer, g_config.max_download_size, metadata.hash);
    metadata.rollback_enabled = g_config.rollback_enabled;
    metadata.boot_attempts = 0;
   
    write_metadata(&metadata);
   
    g_ota_state = OTA_STATE_SWITCHING;
    notify_status(OTA_STATE_SWITCHING, 0);
   
    /* Trigger reboot to apply update */
    ota_switch_firmware();
   
    return 0;
}

int ota_download_firmware(const char *url,
                          uint8_t *buffer,
                          size_t buffer_size)
{
    /*
     * In production, implement HTTP/TLS download with:
     * - Certificate pinning
     * - Chunked download with progress callbacks
     * - Retry logic with exponential backoff
     * - Timeout handling
     */
   
    /* Placeholder for network stack implementation */
    /* Example using lwIP or similar: */
   
    int downloaded = 0;
    size_t chunk_size = 1024;
   
    while (downloaded < buffer_size) {
        /* Download chunk */
        size_t remaining = buffer_size - downloaded;
        size_t to_read = (remaining < chunk_size) ? remaining : chunk_size;
       
        /* Network download call would go here */
        /* int bytes_read = http_download_chunk(url, buffer + downloaded, to_read); */
        int bytes_read = to_read; /* Placeholder */
       
        if (bytes_read <= 0) {
            return -1;
        }
       
        downloaded += bytes_read;
       
        /* Notify progress */
        if (g_progress_cb != NULL) {
            g_progress_cb(downloaded, buffer_size);
        }
       
        /* Check for timeout */
        /* if (elapsed_time() > g_config.timeout_ms) return -1; */
    }
   
    return 0;
}

int ota_verify_firmware(const uint8_t *firmware,
                        size_t size,
                        const uint8_t *signature,
                        size_t signature_len)
{
    /* Compute hash of downloaded firmware */
    uint8_t hash[SHA256_DIGEST_SIZE];
    int hash_result = crypto_sha256(firmware, size, hash);
    if (hash_result != 0) {
        return -1;
    }
   
    /* Verify signature using stored public key */
    uint8_t public_key[ECDSA_PUBKEY_SIZE];
    secure_boot_load_public_key(public_key);
   
    int verify_result = crypto_ecdsa_verify(public_key, hash, signature);
   
    /* Clear sensitive data */
    secure_memzero(hash, sizeof(hash));
    secure_memzero(public_key, sizeof(public_key));
   
    return verify_result;
}

int ota_switch_firmware(void)
{
    /* Update boot partition flag in hardware registers */
    /* Example: STM32 option bytes or NXP eFuse */
   
    /* Set boot partition to pending partition */
    set_boot_partition(g_pending_partition);
   
    /* Reset boot counter for new firmware */
    ota_reset_boot_counter();
   
    /* Reboot device */
    NVIC_SystemReset();
   
    return 0;
}

int ota_rollback(void)
{
    g_ota_state = OTA_STATE_ROLLBACK;
    notify_status(OTA_STATE_ROLLBACK, 0);
   
    /* Switch back to previous partition */
    uint8_t previous_partition = (g_active_partition == 0) ? 1 : 0;
    set_boot_partition(previous_partition);
   
    /* Reset boot counter */
    ota_reset_boot_counter();
   
    /* Reboot */
    NVIC_SystemReset();
   
    return 0;
}

ota_state_t ota_get_state(void)
{
    return g_ota_state;
}

void ota_register_progress_callback(ota_progress_cb cb)
{
    g_progress_cb = cb;
}

void ota_register_status_callback(ota_status_cb cb)
{
    g_status_cb = cb;
}

int ota_check_boot_success(void)
{
    /*
     * Called early in boot sequence.
     * Returns boot counter to determine if rollback needed.
     */
    ota_metadata_t metadata;
    if (read_metadata(&metadata) != 0) {
        return -1;
    }
   
    return (int)metadata.boot_attempts;
}

void ota_increment_boot_counter(void)
{
    ota_metadata_t metadata;
    if (read_metadata(&metadata) != 0) {
        return;
    }
   
    metadata.boot_attempts++;
   
    /* Trigger rollback if threshold exceeded */
    if (metadata.boot_attempts >= g_config.rollback_threshold) {
        if (metadata.rollback_enabled) {
            ota_rollback();
        }
    }
   
    write_metadata(&metadata);
}

void ota_reset_boot_counter(void)
{
    ota_metadata_t metadata;
    if (read_metadata(&metadata) != 0) {
        return;
    }
   
    metadata.boot_attempts = 0;
    write_metadata(&metadata);
}

/* Helper functions (implement based on your hardware) */
static int read_metadata(ota_metadata_t *metadata)
{
    /* Read from flash at METADATA_OFFSET */
    /* return flash_read(METADATA_OFFSET, metadata, sizeof(ota_metadata_t)); */
    return 0;
}

static int write_metadata(const ota_metadata_t *metadata)
{
    /* Write to flash at METADATA_OFFSET */
    /* return flash_write(METADATA_OFFSET, metadata, sizeof(ota_metadata_t)); */
    return 0;
}

static void set_boot_partition(uint8_t partition)
{
    /* Hardware-specific implementation */
    /* Example: STM32 - modify option bytes */
    /* Example: ESP32 - set boot partition flag in NVS */
}

static uint32_t calculate_firmware_version(const uint8_t *firmware, size_t size)
{
    /* Extract version from firmware header */
    /* Return version number */
    return 1;
}

static void notify_status(ota_state_t state, int error_code)
{
    if (g_status_cb != NULL) {
        g_status_cb(state, error_code);
    }
}

static void init_default_metadata(void)
{
    ota_metadata_t metadata;
    memset(&metadata, 0, sizeof(metadata));
    metadata.magic = 0xDEADBEEF;
    metadata.rollback_enabled = 1;
    write_metadata(&metadata);
}

