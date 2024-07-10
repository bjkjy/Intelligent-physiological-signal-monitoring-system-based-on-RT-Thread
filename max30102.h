/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-02-08     Jackistang   the first version
 */
#ifndef PKG_MAX30102_H__
#define PKG_MAX30102_H__

#include <rtthread.h>
#include "sensor.h"
#include "user.h"

#ifndef MAX30102_STACK_SIZE
#define MAX30102_STACK_SIZE 1024
#endif

#ifndef MAX30102_PRIORITY
#define MAX30102_PRIORITY   10
#endif

#ifndef MAX30102_TICKS
#define MAX30102_TICKS      10
#endif

typedef struct max30102_data
{
    int16_t red;
    int16_t ir;
    uint8_t get_finish_flag;
}max30102_data;
typedef max30102_data* max30102_data_ptr;

extern max30102_data_ptr my_data_p;

int rt_hw_max30102_init(struct rt_sensor_config *cfg);
Physiological_signal my_blood_data_translate(void);

#endif /*  MAX30102_H_ */
