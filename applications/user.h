/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-06-27     Baijiankang       the first version
 */
#ifndef APPLICATIONS_USER_H_
#define APPLICATIONS_USER_H_

/*LCD菜单*/
extern rt_int16_t menu_state;

/*血氧测量*/
typedef struct Physiological_signal
{
    rt_int16_t HR;      //定义心率
    float SpO2;     //定义血氧饱和度
    rt_uint8_t is_true;
}Physiological_signal;
extern Physiological_signal blood_result;

/*步数测量和跌倒监测*/
extern rt_int16_t g_step_count;
extern rt_uint8_t g_fall_flag;

/*初始化线程函数声明*/
rt_int16_t init_flash_thread(void);
rt_int16_t init_uart2();
rt_int16_t init_lcd_thread(void);
rt_int16_t init_max30102_port(void);
rt_int16_t init_key_led_thread(void);
rt_int16_t init_step_thread(void);

#endif /* APPLICATIONS_USER_H_ */
