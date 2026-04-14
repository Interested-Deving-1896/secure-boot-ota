/*
 * This file is part of [secure-boot-ota].
 *
 * Copyright (C) [2026] [Maximilian Feldthusen].
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

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
