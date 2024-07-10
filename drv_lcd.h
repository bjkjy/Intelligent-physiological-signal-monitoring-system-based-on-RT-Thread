/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-06-27     Baijiankang       the first version
 */
#ifndef DRIVERS_INCLUDE_DRV_LCD_H_
#define DRIVERS_INCLUDE_DRV_LCD_H_

#include <rtthread.h>

#define LCD_DISPLAY_DIR 2

#if LCD_DISPLAY_DIR == 0 || LCD_DISPLAY_DIR == 1
  #define LCD_TFTWIDTH  128
  #define LCD_TFTHEIGHT 160
#else
  #define LCD_TFTWIDTH  160
  #define LCD_TFTHEIGHT 128
#endif

#define RED     0xF800
#define BLUE    0x001F
#define YELLOW  0xFFE0
#define GREEN   0x07E0
#define WHITE   0xFFFF
#define BLACK   0x0000
#define GRAY    0X8430
#define BROWN   0XBC40
#define PURPLE  0XF81F
#define PINK    0XFE19

#define PEN_COLOR  PURPLE
#define BG_COLOR   WHITE

extern rt_uint8_t lcd_init_ok;
//int lcd_init(const char *dev_name, rt_base_t dc_pin, rt_base_t reset_pin);
void lcd_reset(void);
void lcd_fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color);
void lcd_clear(uint16_t color);
void lcd_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void lcd_draw_rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color);
void lcd_draw_circ(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);
void lcd_showchar(uint16_t x, uint16_t y, uint8_t dat);
void lcd_showstr(uint16_t x, uint16_t y, uint8_t dat[]);
void lcd_showint8(uint16_t x, uint16_t y, int8_t dat);
void lcd_showuint8(uint16_t x,uint16_t y,uint8_t dat);
void lcd_showint16(uint16_t x,uint16_t y,int16_t dat);
void lcd_showuint16(uint16_t x,uint16_t y,uint16_t dat);
void lcd_showint32(uint16_t x, uint16_t y, int32_t dat, uint8_t num);
void lcd_showfloat(uint16_t x, uint16_t y, double dat, uint8_t num, uint8_t pointnum);
void lcd_show_chinese(uint16_t x, uint16_t y, uint8_t index, uint16_t color);
void lcd_showpicture(uint16_t x,uint16_t y,uint16_t width,uint16_t hight,const uint8_t picture[]);

#endif /* DRIVERS_INCLUDE_DRV_LCD_H_ */
