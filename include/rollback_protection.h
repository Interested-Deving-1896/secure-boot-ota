#ifndef ROLLBACK_PROTECTION_H
#define ROLLBACK_PROTECTION_H

#include <stdint.h>
#include <stddef.h>
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t version_counter;
    uint8_t signature[64];
    uint64_t last_updated;
    uint8_t flags;
    uint8_t reserved[3];
} rollback_protection_t;

int rollback_init(rollback_protection_t *rb);
int rollback_check_and_increment(rollback_protection_t *rb,
                                uint32_t min_version);
int rollback_get_current_version(rollback_protection_t *rb,
                                uint32_t *version);
int rollback_verify_signature(rollback_protection_t *rb);
int rollback_sign(rollback_protection_t *rb);
void rollback_cleanup(rollback_protection_t *rb);

#ifdef __cplusplus
}
#endif

#endif /* ROLLBACK_PROTECTION_H */

