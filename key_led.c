/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-07-09     Baijiankang       the first version
 */

#include <rtthread.h>
#include <drv_common.h>
#include "user.h"

#define KEY1 GET_PIN(E,1)
#define KEY2 GET_PIN(A,11)
#define KEY3 GET_PIN(A,12)
#define LED1 GET_PIN(E,5)
#define LED2 GET_PIN(E,2)
#define LED3 GET_PIN(A,2)

#define LED1_ON rt_pin_write(LED1, PIN_LOW)
#define LED1_OFF rt_pin_write(LED1, PIN_HIGH)
#define LED2_ON rt_pin_write(LED2, PIN_LOW)
#define LED2_OFF rt_pin_write(LED2, PIN_HIGH)
#define LED3_ON rt_pin_write(LED3, PIN_LOW)
#define LED3_OFF rt_pin_write(LED3, PIN_HIGH)

void key_led_GPIO_init(void)
{
    rt_pin_mode(KEY1, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(KEY2, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(KEY3, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(LED1, PIN_MODE_OUTPUT);
    rt_pin_mode(LED2, PIN_MODE_OUTPUT);
    rt_pin_mode(LED3, PIN_MODE_OUTPUT);
    LED1_OFF;
    LED2_OFF;
    LED3_OFF;
    rt_kprintf("key & led init successfully!\n");
}

void key_led_test1(void)
{
    if (rt_pin_read(KEY1) == PIN_LOW)
    {
        LED1_ON;
    }
    else
    {
        LED1_OFF;
    }

    if (rt_pin_read(KEY2) == PIN_LOW)
    {
        LED2_ON;
    }
    else
    {
        LED2_OFF;
    }

    if (rt_pin_read(KEY3) == PIN_LOW)
    {
        LED3_ON;
    }
    else
    {
        LED3_OFF;
    }
}

void water_led_show(int16_t time)
{
    LED1_ON;
    LED2_OFF;
    LED3_OFF;
    rt_thread_mdelay(time);
    LED1_OFF;
    LED2_ON;
    LED3_OFF;
    rt_thread_mdelay(time);
    LED1_OFF;
    LED2_OFF;
    LED3_ON;
    rt_thread_mdelay(time);
}

static int key_scan(void)
{
    if (rt_pin_read(KEY1) == PIN_LOW)
    {
        rt_thread_mdelay(10);
        if (rt_pin_read(KEY1) == PIN_LOW)
        {
            while(rt_pin_read(KEY1) == PIN_LOW);
            return 1;
        }
    }

    if (rt_pin_read(KEY2) == PIN_LOW)
    {
        rt_thread_mdelay(10);
        if (rt_pin_read(KEY2) == PIN_LOW)
        {
            while(rt_pin_read(KEY2) == PIN_LOW);
            return 2;
        }
    }

    if (rt_pin_read(KEY3) == PIN_LOW)
    {
        rt_thread_mdelay(10);
        if (rt_pin_read(KEY3) == PIN_LOW)
        {
            while(rt_pin_read(KEY3) == PIN_LOW);
            return 3;
        }
    }
        return 0;
}

/*线程入口函数*/
static void key_led_thread_entry(void *parameter)
{
    int key_num = 0;
    key_led_GPIO_init();
    while (1)
    {    
        key_num = key_scan();

        if (key_num == 2 && menu_state >= 1)
         {
             menu_state --;
         }
        else if (key_num == 3 && menu_state < 2)
         {
            menu_state ++;
         }

        rt_thread_mdelay(50);
    }
}

rt_int16_t init_key_led_thread(void)
{
    rt_thread_t key_led_thread;

    key_led_thread = rt_thread_create("key_led",
                                    key_led_thread_entry, 
                                    RT_NULL, 
                                    1024,
                                    11,
                                    10);

    if (key_led_thread != RT_NULL)
    {
        rt_thread_startup(key_led_thread);
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
}
// INIT_APP_EXPORT(key_led_thread_init);


