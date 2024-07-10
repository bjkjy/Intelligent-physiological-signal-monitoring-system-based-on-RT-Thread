/*
 * Copyright (c) 2006-2024, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-06-30     RT-Thread    first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "uart.h"
#include "user.h"

/**
 * @brief The main function of the project
 * 
 * @return int 
 */
int main(void)
{
    /* 初始化串口2 */
    init_uart2();
    /* 初始化并启动各个线程 */
    init_flash_thread();
    init_lcd_thread();
    init_key_led_thread();
    init_max30102_port();
    init_step_thread();
    
    while(1)
    {
        rt_thread_mdelay(1000);
    }
    return RT_EOK;
}
