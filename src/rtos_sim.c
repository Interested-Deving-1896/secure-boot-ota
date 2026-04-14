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

