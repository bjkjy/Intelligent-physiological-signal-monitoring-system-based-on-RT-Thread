/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-06-27     Baijiankang       the first version
 */

#include <rtthread.h>
#include "drv_lcd.h"
#include "lcd_font.h"
#include "user.h"
#include "uart.h"

/* LCD测试函数 */
#define Rect_W 30
#define Rect_H 24
/**
 * @brief LCD测试函数1
 * 
 */
void lcd_test_1()
{
    lcd_clear(WHITE);
    lcd_draw_rect(40 - Rect_W, 32 - Rect_H, 40 + Rect_W, 32 + Rect_H, BLUE);
    lcd_draw_rect(120 - Rect_W, 32 - Rect_H, 120 + Rect_W, 32 + Rect_H, YELLOW);
    lcd_draw_rect(40 - Rect_W, 96 - Rect_H, 40 + Rect_W, 96 + Rect_H, GRAY);
    lcd_draw_rect(120 - Rect_W, 96 - Rect_H, 120 + Rect_W, 96 + Rect_H, GREEN);
    lcd_draw_rect(80 - Rect_W, 64 - Rect_H, 80 + Rect_W, 64 + Rect_H, PINK);

    rt_thread_mdelay(1000);
    lcd_clear(WHITE);

    lcd_draw_circ(80, 64, 5 * 2, RED);
    lcd_draw_circ(80, 64, 10 * 2, GREEN);
    lcd_draw_circ(80, 64, 15 * 2, BLUE);
    lcd_draw_circ(80, 64, 20 * 2, GRAY);
    lcd_draw_circ(80, 64, 25 * 2, BROWN);
    lcd_draw_circ(80, 64, 30 * 2, PINK);

    rt_thread_mdelay(1000);
    lcd_clear(WHITE);

    lcd_showpicture(0, 0, 160, 80, RT_Thread_logo_map);
    lcd_showstr(20, 5, (uint8_t *)"Powered by");
    lcd_showstr(40, 6, (uint8_t *)"RT-Thread bjk");
}

/**
 * @brief LCD测试函数2
 * 
 */
void lcd_test_2()
{
    rt_tick_t tick_start, tick_end;
    tick_start = rt_tick_get();
    for(uint8_t i = 0; i < 2; ++i)
    {
        lcd_clear(RED);
        lcd_clear(GREEN);
        lcd_clear(BLUE);
    }
    tick_end = rt_tick_get();
    rt_kprintf("clear screen use %d ms\n", (tick_end - tick_start) / 9);
    lcd_clear(WHITE);
}

/**
 * @brief 获取并显示FPS
 * 
 * @return int16_t 
 */
int16_t with_rtthread_get_show_fps()
{
   uint8_t count = 100;
   uint8_t show_data = 48;
   rt_tick_t tick_start, tick_end;
   tick_start = rt_tick_get();
   while(count--)
   {
       lcd_showchar(10, 10, show_data);
       lcd_showchar(10, 10, show_data + 1);
       lcd_showchar(10, 10, show_data + 2);
       lcd_showchar(10, 10, show_data + 3);
       lcd_showchar(10, 10, show_data + 4);
       lcd_showchar(10, 10, show_data + 5);
       lcd_showchar(10, 10, show_data + 6);
       lcd_showchar(10, 10, show_data + 7);
       lcd_showchar(10, 10, show_data + 8);
       lcd_showchar(10, 10, show_data + 9);
   }
   tick_end = rt_tick_get();
   lcd_clear(WHITE);
   return (1000/((tick_end - tick_start) / 1000));
}

int last_state = -1;
/**
 * @brief 刷新显示屏
 * 
 * @param menu_state 
 */
void tft_refresh(int16_t menu_state)
{
    if(menu_state != last_state)
        lcd_clear(WHITE);
    last_state = menu_state;
}

/**
 * @brief 测试函数
 * 
 */
void bluetooth_test_by_lcd()
{
    if(g_type_of_recv_uart2 == NUMBER){
        lcd_showfloat(10,3,g_received_float_uart2, 5, 5);
        lcd_showstr(10, 6, (uint8_t*)"NUMBER");
    }
    else if (g_type_of_recv_uart2 == STRING) {
        lcd_showstr(10, 3, (uint8_t*)g_received_string_uart2);
        lcd_showstr(10, 6, (uint8_t*)"STRING");
    }
}

int16_t menu_state = 0;
/**
 * @brief 显示菜单
 * 
 */
void lcd_menu_display()
{
    if(menu_state ==0)
    {
        tft_refresh(0);
        lcd_showpicture(0, 0, 160, 80, RT_Thread_logo_map);
        uint8_t index = 0;
        lcd_show_chinese(25, 80, index, 0x001F); // 在(25, 80)位置显示蓝色“嵌”
        lcd_show_chinese(45, 80, index+1, 0x001F);
        lcd_show_chinese(65, 80, index+2, 0x001F);
        lcd_show_chinese(85, 80, index+3, 0x001F);
        lcd_show_chinese(105, 80, index+4, 0x001F);
        lcd_show_chinese(25, 100, index+5, 0x001F);
        lcd_show_chinese(45, 100, index+6, 0x001F);
        lcd_show_chinese(65, 100, index+7, 0x001F);
        lcd_show_chinese(85, 100, index+8, 0x001F);
        lcd_show_chinese(105, 100, index+9, 0x001F);
    }
    else if(menu_state ==1)
    {
        tft_refresh(1);
        // 在右侧显示心率、血氧和步数
        lcd_showstr(20, 1, (uint8_t *)"HR:");
        lcd_showuint8(80, 1, (uint8_t)blood_result.HR);

        lcd_showstr(20, 3, (uint8_t *)"SpO2:");
        lcd_showuint8(80, 3, (uint8_t)blood_result.SpO2);

        lcd_showstr(20, 5, (uint8_t *)"Steps:");
        lcd_showuint8(80, 5, (uint8_t)g_step_count);
    }
    else if(menu_state == 2)
    {
        tft_refresh(2);
        if(blood_result.HR > 90)
            {
                lcd_showstr(20, 0, (uint8_t *)"Your heart rate  ");
                lcd_showstr(20, 1, (uint8_t *)"is a little high!");
            }
        else
            {
                lcd_showstr(20, 0, (uint8_t *)"Your heart rate  ");
                lcd_showstr(20, 1, (uint8_t *)"is normal!       ");
            }
        if(blood_result.SpO2 < 94)
            {
                lcd_showstr(20, 3, (uint8_t *)"Your blood oxygen");
                lcd_showstr(20, 4, (uint8_t *)"is a little low! ");
            }
        else
            {
                lcd_showstr(20, 3, (uint8_t *)"Your blood oxygen");
                lcd_showstr(20, 4, (uint8_t *)"is normal.         ");
            }

        if(g_step_count < 2000)
            {
                lcd_showstr(20, 6, (uint8_t *)"You're not up to");
                lcd_showstr(20, 7, (uint8_t *)"your goal!      ");
            }
        else
            {
                lcd_showstr(20, 6, (uint8_t *)"You've reached  ");
                lcd_showstr(20, 7, (uint8_t *)"your goal.      ");
            }
    }
}

/**
 * @brief LCD线程入口函数
 * 
 * @param p 
 */
void lcd_thread_entry(void *p)
{
    rt_thread_mdelay(1000);
    while(1)
    {
        if(lcd_init_ok)
        {
            lcd_menu_display();
        }
        rt_thread_mdelay(100);
    }
}

/**
 * @brief 初始化LCD线程
 * 
 * @return rt_int16_t 
 */
rt_int16_t init_lcd_thread(void)
{
    rt_thread_t lcd_thread;
    // 创建线程，并启动它
    //-----------------线程------------------------------------------
    lcd_thread = rt_thread_create("lcd_show", lcd_thread_entry, RT_NULL, 2048, 15, 5);
    if(lcd_thread != RT_NULL)
        {
            rt_thread_startup(lcd_thread); // 如果线程创建成功，则启动它
            rt_kprintf("lcd_show_successed!\n");
        }
    else
        rt_kprintf("lcd_show_failed!\n"); // 如果线程创建失败，则打印错误信息
    return RT_EOK;
}
//INIT_DEVICE_EXPORT(lcd_thread);
