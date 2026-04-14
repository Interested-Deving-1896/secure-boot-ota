
This project is **dual-licensed** under two licenses:

1. **GPL v3 (GNU General Public License v3.0)**:
   - You are free to use, modify, and distribute this software under the terms of the GPL v3 license.
   - See the [LICENSE](./LICENSE) file for the full text of the GPL v3 license.
   - If you modify and distribute this software, you must release your modifications under the same GPL v3 license.

2. **Commercial License**:
   - If you wish to use the software for commercial purposes **without the requirements of the GPL**, you may obtain a commercial license.
   - The commercial license provides more flexibility for use in proprietary applications.
   - Please see the [COMMERCIAL_LICENCE.md](./COMMERCIAL_LICENSE.md) file for details on obtaining a commercial license.

### Which License Should You Choose?

- **Choose GPL v3** if you are working on open-source projects or if you are happy to share your modifications under the same license.
- **Choose the Commercial License** if you need to use the software in a proprietary or closed-source project, or if you want to avoid the copyleft requirements of the GPL.

If you have any questions about which license applies to your use case, or if you would like to discuss commercial licensing options, please contact us at [info@mafeforge.com].



---




 Filesystem (realistic embedded layout)
secure-boot-ota-fw/
├── include/
│   ├── config.h
│   ├── platform.h
│   ├── rtos_sim.h
│   ├── crypto.h
│   ├── secure_boot.h
│   └── ota.h
├── src/
│   ├── main.c
│   ├── rtos_sim.c
│   ├── crypto.c
│   ├── secure_boot.c
│   └── ota.c
├── Makefile
└── README.md


📄 include/config.h
#ifndef CONFIG_H
#define CONFIG_H

// Device identity (would normally come from OTP/flash)
#define DEVICE_ID          "dev-001"

// Firmware
#define FW_VERSION         "1.0.0"

// Memory constraints (simulated target)
#define MAX_FW_SIZE        (512 * 1024)   // 512 KB
#define HASH_SIZE          32
#define SIG_SIZE           64

// OTA
#define OTA_CHECK_INTERVAL_MS   10000

#endif


include/platform.h
#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

/*
 * Platform abstraction (very thin)
 * In real firmware this wraps HAL / BSP
 */

void platform_log(const char *fmt, ...);

// TODO: replace with actual flash driver
int flash_write(uint32_t addr, const uint8_t *data, uint32_t len);
int flash_read(uint32_t addr, uint8_t *data, uint32_t len);

#endif


 include/rtos_sim.h
#ifndef RTOS_SIM_H
#define RTOS_SIM_H

#include <stdint.h>

typedef void *(*task_fn_t)(void *);

int rtos_create_task(task_fn_t fn, const char *name, void *arg);
void rtos_delay(uint32_t ms);

#endif


include/crypto.h
#ifndef CRYPTO_H
#define CRYPTO_H

#include <stdint.h>
#include <stddef.h>

int crypto_init(void);

// WARNING: placeholder only
int crypto_hash(const uint8_t *data, size_t len, uint8_t *out);
int crypto_verify(const uint8_t *hash, const uint8_t *sig);

#endif


include/secure_boot.h
#ifndef SECURE_BOOT_H
#define SECURE_BOOT_H

#include <stdint.h>

typedef enum {
    BOOT_UNKNOWN = 0,
    BOOT_OK,
    BOOT_FAIL
} boot_state_t;

typedef struct {
    boot_state_t state;
} secure_boot_ctx_t;

int secure_boot_init(secure_boot_ctx_t *ctx);
int secure_boot_verify(secure_boot_ctx_t *ctx,
                       const uint8_t *image,
                       uint32_t len);

#endif


include/ota.h
#ifndef OTA_H
#define OTA_H

#include <stdint.h>
#include "secure_boot.h"

typedef enum {
    OTA_IDLE = 0,
    OTA_DOWNLOADING,
    OTA_VERIFYING,
    OTA_DONE,
    OTA_ERROR
} ota_state_t;

typedef struct {
    ota_state_t state;
    char current_version[16];
} ota_ctx_t;

int ota_init(ota_ctx_t *ctx, secure_boot_ctx_t *sb);
int ota_check(ota_ctx_t *ctx);
int ota_perform_update(ota_ctx_t *ctx);

#endif


src/rtos_sim.c
#include "rtos_sim.h"
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

int rtos_create_task(task_fn_t fn, const char *name, void *arg)
{
    pthread_t t;

    if (pthread_create(&t, NULL, fn, arg) != 0) {
        printf("rtos: failed to start %s\n", name);
        return -1;
    }

    pthread_detach(t);
    printf("rtos: started %s\n", name);
    return 0;
}

void rtos_delay(uint32_t ms)
{
    usleep(ms * 1000);
}


src/crypto.c
#include "crypto.h"
#include <string.h>

/*
 * NOTE:
 * This is NOT real crypto.
 * Replace with mbedTLS or hardware crypto engine.
 */

int crypto_init(void)
{
    return 0;
}

int crypto_hash(const uint8_t *data, size_t len, uint8_t *out)
{
    if (!data || !out) return -1;

    memset(out, 0, HASH_SIZE);

    for (size_t i = 0; i < len; i++) {
        out[i % HASH_SIZE] ^= data[i];
    }

    return 0;
}

int crypto_verify(const uint8_t *hash, const uint8_t *sig)
{
    // fake check
    return (hash && sig);
}


src/secure_boot.c
#include "secure_boot.h"
#include "crypto.h"
#include <string.h>

/*
 * Secure boot (simplified)
 * Real system would verify against ROM-stored hash/key
 */

int secure_boot_init(secure_boot_ctx_t *ctx)
{
    if (!ctx) return -1;

    memset(ctx, 0, sizeof(*ctx));
    ctx->state = BOOT_UNKNOWN;
    return 0;
}

int secure_boot_verify(secure_boot_ctx_t *ctx,
                       const uint8_t *image,
                       uint32_t len)
{
    if (!ctx || !image || len == 0) {
        ctx->state = BOOT_FAIL;
        return -1;
    }

    // TODO: compare against trusted hash in flash/ROM

    ctx->state = BOOT_OK;
    return 0;
}


 src/ota.c
#include "ota.h"
#include "crypto.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
 * OTA logic
 *
 * NOTE:
 * - currently RAM-based (not realistic for big firmware)
 * - should stream directly to flash
 */

static secure_boot_ctx_t *g_sb = NULL;

int ota_init(ota_ctx_t *ctx, secure_boot_ctx_t *sb)
{
    if (!ctx || !sb) return -1;

    memset(ctx, 0, sizeof(*ctx));
    strncpy(ctx->current_version, FW_VERSION, sizeof(ctx->current_version)-1);

    g_sb = sb;
    ctx->state = OTA_IDLE;
    return 0;
}

int ota_check(ota_ctx_t *ctx)
{
    if (!ctx) return -1;

    // Stub: always "update available"
    return 1;
}

int ota_perform_update(ota_ctx_t *ctx)
{
    if (!ctx) return -1;

    ctx->state = OTA_DOWNLOADING;

    size_t fw_len = 256 * 1024; // 256 KB test image

    if (fw_len > MAX_FW_SIZE) {
        ctx->state = OTA_ERROR;
        return -1;
    }

    uint8_t *fw = malloc(fw_len);
    if (!fw) {
        ctx->state = OTA_ERROR;
        return -1;
    }

    memset(fw, 0xAA, fw_len);

    ctx->state = OTA_VERIFYING;

    uint8_t hash[HASH_SIZE];
    crypto_hash(fw, fw_len, hash);

    if (!crypto_verify(hash, hash)) {
        ctx->state = OTA_ERROR;
        free(fw);
        return -1;
    }

    // TODO: write to inactive partition (flash driver missing)

    ctx->state = OTA_DONE;
    free(fw);

    return 0;
}


 src/main.c (RTOS-style)
#include <stdio.h>
#include "rtos_sim.h"
#include "secure_boot.h"
#include "ota.h"
#include "crypto.h"
#include "config.h"

static secure_boot_ctx_t sb;
static ota_ctx_t ota;

/* System task */
void *sys_task(void *arg)
{
    (void)arg;

    crypto_init();
    secure_boot_init(&sb);
    ota_init(&ota, &sb);

    uint8_t dummy[] = "boot";
    secure_boot_verify(&sb, dummy, sizeof(dummy));

    while (1) {
        printf("[SYS] boot=%d\n", sb.state);
        rtos_delay(5000);
    }
}

/* OTA task */
void *ota_task(void *arg)
{
    (void)arg;

    while (1) {
        rtos_delay(OTA_CHECK_INTERVAL_MS);

        if (ota_check(&ota)) {
            printf("[OTA] update found\n");

            if (ota_perform_update(&ota) == 0) {
                printf("[OTA] update done\n");
            } else {
                printf("[OTA] update failed\n");
            }
        }
    }
}

int main(void)
{
    printf("fw start (%s)\n", FW_VERSION);

    rtos_create_task(sys_task, "sys", NULL);
    rtos_create_task(ota_task, "ota", NULL);

    while (1) {
        rtos_delay(1000); // idle
    }

    return 0;
}


Makefile
CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -Iinclude

SRC=$(wildcard src/*.c)
OUT=fw

all:
	$(CC) $(CFLAGS) $(SRC) -lpthread -o $(OUT)

clean:
	rm -f $(OUT)


 README.md (human, embedded tone)
Secure Boot + OTA (Embedded-style demo)

Notes:
- This is NOT production-ready.
- Crypto is stubbed (XOR-based).
- OTA uses RAM buffer (not suitable for real devices).
- No actual networking yet.

Intended direction:
- Replace crypto with mbedTLS
- Stream OTA directly to flash
- Add TLS + cert validation
- Hook into real RTOS (FreeRTOS)

Build:
  make
  ./fw


 

 Secure Boot + OTA Firmware (Explained)
 Overview

This project simulates a real embedded firmware system with:

    Secure boot verification

    OTA (Over-The-Air) updates

    RTOS-style multitasking (via POSIX threads)

    Hardware abstraction (flash, logging)

It is intentionally not production-ready, but structured like real firmware under development.
 Architecture

+---------------------+
|      main.c         |
|---------------------|
|  sys_task           |
|  ota_task           |
+----------+----------+
           |
           v
+---------------------+
|    rtos_sim.c       |  -> pthread wrapper
+---------------------+

+---------------------+       +----------------------+
|  secure_boot.c      |<----->|     crypto.c         |
+---------------------+       +----------------------+

+---------------------+
|      ota.c          |
+---------------------+

+---------------------+
|   platform (stub)   |
+---------------------+

 File-by-File Explanation
 config.h

Defines system-wide constants:

#define DEVICE_ID "dev-001"
#define FW_VERSION "1.0.0"
#define MAX_FW_SIZE (512 * 1024)

 Represents flash + device constraints.
 platform.h

Hardware abstraction layer (HAL):

int flash_write(uint32_t addr, const uint8_t *data, uint32_t len);

 Currently stubbed — no real flash driver.
 rtos_sim.*

Simulates an RTOS using pthread.
Key idea:

rtos_create_task(fn, name, arg);

Internally:

pthread_create(...)

 Mimics FreeRTOS task creation, but runs on Linux.
 crypto.*

 Fake crypto implementation
Hash:

out[i % HASH_SIZE] ^= data[i];

    XOR-based

    NOT secure

    Placeholder for:

        mbedTLS

        hardware crypto engine

Verify:

return (hash && sig);

Always passes if pointers exist.
📄 secure_boot.*

Simulates firmware verification.

ctx->state = BOOT_OK;

 Missing real logic:

    No signature validation

    No ROM-stored key

    No anti-rollback

 ota.*

Handles firmware updates.
Flow:

IDLE
  ↓
DOWNLOADING (malloc)
  ↓
VERIFYING (fake crypto)
  ↓
DONE / ERROR

Key issues (intentional realism):

    Uses RAM buffer (bad for real devices)

    No networking

    No flash writes

 main.c

Creates two RTOS-style tasks:
 sys_task

crypto_init();
secure_boot_init();
ota_init();
secure_boot_verify();

Loop:

[SYS] boot=1

 ota_task

if (ota_check()) {
    ota_perform_update();
}

Runs every:

OTA_CHECK_INTERVAL_MS = 10000 ms

 Runtime Behavior
Program Start

fw start (1.0.0)
rtos: started sys
rtos: started ota

 System Task Output (every 5s)

[SYS] boot=1

Meaning:

    BOOT_OK = 1

 OTA Task Output (every 10s)

[OTA] update found
[OTA] update done

 Why always?

ota_check() → always returns 1

📊 Simulated Execution Timeline

Time (s)   Event
--------   ------------------------
0          Firmware starts
0          Tasks created
5          [SYS] boot=1
10         OTA check → update
10         [OTA] update found
10         [OTA] update done
10         [SYS] boot=1
15         [SYS] boot=1
20         OTA runs again

 Embedded Realism Notes
 Not realistic (yet)

    Full firmware loaded into RAM

    No flash partitioning

    No watchdog

    No rollback mechanism

    No TLS / networking

 Real firmware would add

    Dual-bank flash (A/B)

    Secure key storage (OTP / eFuse)

    TLS download

    Streaming OTA

    Bootloader verification stage

 Build & Run

make
./fw

 Final Output Example

fw start (1.0.0)
rtos: started sys
rtos: started ota
[SYS] boot=1
[OTA] update found
[OTA] update done
[SYS] boot=1

 Key Takeaways

    Structure mimics real embedded firmware

    Clear separation of:

        boot

        crypto

        OTA

        RTOS

    Designed to evolve into:

        FreeRTOS

        Real hardware target (ESP32 / STM32)

 
    




This project simulates a structured embedded firmware system with:

- Secure boot verification
- OTA (Over-The-Air) updates
- RTOS-style multitasking (via POSIX threads)
- Hardware abstraction (flash, logging)

It is intentionally not production-ready, but resembles real firmware under development.

---

## Architecture

+---------------------+
main.c
sys_task
ota_task
+----------+----------+

       |
       v

+---------------------+
| rtos_sim.c | -> pthread wrapper
+---------------------+

+---------------------+ +----------------------+
| secure_boot.c |<----->| crypto.c |
+---------------------+ +----------------------+

+---------------------+
| ota.c |
+---------------------+

+---------------------+
| platform (stub) |
+---------------------+


---

## File-by-File Explanation

### config.h

Defines system-wide constants:

```c
#define DEVICE_ID "dev-001"
#define FW_VERSION "1.0.0"
#define MAX_FW_SIZE (512 * 1024)

Represents flash and device constraints.
platform.h

Hardware abstraction layer (HAL):

int flash_write(uint32_t addr, const uint8_t *data, uint32_t len);

Currently stubbed; no real flash driver is implemented.
rtos_sim.*

Simulates an RTOS using pthread.

Key API:

rtos_create_task(fn, name, arg);

Internally uses:

pthread_create(...)

Mimics FreeRTOS-style task creation on a desktop system.
crypto.*

Fake crypto implementation (placeholder only).

Hash function:

out[i % HASH_SIZE] ^= data[i];

    XOR-based

    Not secure

    Intended to be replaced with mbedTLS or hardware crypto

Verification:

return (hash && sig);

Always succeeds if pointers are non-null.
secure_boot.*

Simulates firmware verification.

ctx->state = BOOT_OK;

Missing real-world features:

    Signature validation

    Trusted key storage (ROM/OTP)

    Anti-rollback protection

ota.*

Handles firmware updates.

State flow:

IDLE
  ↓
DOWNLOADING (malloc)
  ↓
VERIFYING (fake crypto)
  ↓
DONE / ERROR

Known limitations:

    Uses RAM buffer (not realistic for large firmware)

    No networking

    No flash write implementation

main.c

Creates two RTOS-style tasks.
sys_task

crypto_init();
secure_boot_init();
ota_init();
secure_boot_verify();

Loop output:

[SYS] boot=1

ota_task

if (ota_check()) {
    ota_perform_update();
}

Runs every:

OTA_CHECK_INTERVAL_MS = 10000

Runtime Behavior
Program Start

fw start (1.0.0)
rtos: started sys
rtos: started ota

System Task Output (every 5 seconds)

[SYS] boot=1

Indicates:

    BOOT_OK = 1

OTA Task Output (every 10 seconds)

[OTA] update found
[OTA] update done

Reason:

ota_check() → always returns 1

Simulated Execution Timeline

Time (s)   Event
--------   ------------------------
0          Firmware starts
0          Tasks created
5          [SYS] boot=1
10         OTA check → update
10         [OTA] update found
10         [OTA] update done
10         [SYS] boot=1
15         [SYS] boot=1
20         OTA runs again

Embedded Realism Notes
Not realistic (yet)

    Full firmware loaded into RAM

    No flash partitioning

    No watchdog timer

    No rollback mechanism

    No TLS or networking

Real firmware would include

    Dual-bank flash (A/B partitions)

    Secure key storage (OTP / eFuse)

    TLS-based download

    Streaming OTA (no full RAM buffering)

    Bootloader-stage verification

Build & Run

make
./fw

Final Output Example

fw start (1.0.0)
rtos: started sys
rtos: started ota
[SYS] boot=1
[OTA] update found
[OTA] update done
[SYS] boot=1

Key Takeaways

    Structure resembles real embedded firmware

    Separation of concerns:

        secure boot

        crypto

        OTA

        RTOS

    Designed to evolve toward real hardware platforms





