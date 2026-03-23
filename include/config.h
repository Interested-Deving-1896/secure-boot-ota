#ifndef CONFIG_H
#define CONFIG_H

/* Device Configuration */
#define DEVICE_ID           "device-001"
#define FIRMWARE_VERSION    "1.0.0"
#define MAX_VERSION_LEN     32
#define MAX_PATH_LEN        256

/* Security Configuration */
#define KEY_SIZE            32
#define HASH_SIZE           32
#define SIGNATURE_SIZE      64

/* OTA Configuration */
#define OTA_SERVER_URL      "https://firmware.example.com"
#define UPDATE_CHECK_INTERVAL 86400
#define BOOT_TIMEOUT        30
#define MAX_ROLLBACK_ATTEMPTS 3

/* Partition Configuration */
#define PARTITION_ACTIVE    0
#define PARTITION_INACTIVE  1
#define PARTITION_COUNT     2

/* Logging Levels */
#define LOG_LEVEL_ERROR     0
#define LOG_LEVEL_WARN      1
#define LOG_LEVEL_INFO      2
#define LOG_LEVEL_DEBUG     3

#endif /* CONFIG_H */

