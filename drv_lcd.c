/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-01-08     zylx         first version
 */

#include <board.h>
#include <rtdevice.h>
#include <drv_common.h>
#include <lcd_font.h>
#include <drv_lcd.h>

#include <lcd_port.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

#define LOG_TAG             "drv.lcd"
#include <drv_log.h>
#include <drv_spi.h>

/***********************************************/
static struct rt_spi_device *spi_dev_lcd;

rt_uint8_t lcd_init_ok = 0;

static void lcd_spi_init(void)
{
    rt_hw_spi_device_attach(LCD_SPI_BUS, LCD_SPI_DEV_NAME, LCD_CS_PORT, LCD_CS_PIN);
    spi_dev_lcd = (struct rt_spi_device *)rt_device_find(LCD_SPI_DEV_NAME);

    struct rt_spi_configuration cfg;
    cfg.data_width = 8;
    cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_0 |RT_SPI_MSB;
    cfg.max_hz = 42 * 1000 * 1000;
    rt_spi_configure(spi_dev_lcd, &cfg);
}

static void lcd_gpio_init(void)
{
    rt_pin_mode(LCD_CS_GET_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(LCD_DC_GET_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(LCD_RST_GET_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(LCD_BLK_GET_PIN, PIN_MODE_OUTPUT);

    LCD_BLK_LOW;
    LCD_RST_LOW;
    rt_thread_mdelay(100);
    LCD_RST_HIGH;
}

static void lcd_write_cmd(rt_uint8_t cmd)
{
    LCD_DC_LOW;
    rt_spi_send(spi_dev_lcd, &cmd, 1);
}

static void lcd_write_data(rt_uint8_t data)
{
    LCD_DC_HIGH;
    rt_spi_send(spi_dev_lcd, &data, 1);
}

/**
 * @brief  send data to lcd(发送数据)
 *
 * @param data
 * @param data_len
 */
static void lcd_send_data(rt_uint8_t *data, rt_size_t data_len)
{
    LCD_DC_HIGH;
    rt_spi_send(spi_dev_lcd, data, data_len);
}

/**
 * @brief  send command and data to lcd(发送命令和数据)
 *
 * @param cmd
 * @param data
 * @param data_len
 */
static void lcd_send_command_data(rt_uint8_t cmd, rt_uint8_t *data, rt_size_t data_len)
{
    /* Set D/C pin to 0 */
    LCD_DC_LOW;
    lcd_write_cmd(cmd);

    if (data_len)
    {
        LCD_DC_HIGH;
        rt_spi_send(spi_dev_lcd, data, data_len);
    }
}

#define lcd_writeIndex(x) lcd_write_cmd(x)
#define lcd_writeData(x)  lcd_write_data(x)
int lcd_init(void)
{
    lcd_spi_init();
    lcd_gpio_init();

    rt_thread_mdelay(10);
    LCD_RST_HIGH;
    rt_thread_mdelay(120);
    lcd_writeIndex(0x11);
    rt_thread_mdelay(120);

    lcd_writeIndex(0xB1);
    lcd_writeData(0x01);
    lcd_writeData(0x2C);
    lcd_writeData(0x2D);

    lcd_writeIndex(0xB2);
    lcd_writeData(0x01);
    lcd_writeData(0x2C);
    lcd_writeData(0x2D);

    lcd_writeIndex(0xB3);
    lcd_writeData(0x01);
    lcd_writeData(0x2C);
    lcd_writeData(0x2D);
    lcd_writeData(0x01);
    lcd_writeData(0x2C);
    lcd_writeData(0x2D);

    lcd_writeIndex(0xB4);
    lcd_writeData(0x07);

    lcd_writeIndex(0xC0);
    lcd_writeData(0xA2);
    lcd_writeData(0x02);
    lcd_writeData(0x84);
    lcd_writeIndex(0xC1);
    lcd_writeData(0xC5);

    lcd_writeIndex(0xC2);
    lcd_writeData(0x0A);
    lcd_writeData(0x00);

    lcd_writeIndex(0xC3);
    lcd_writeData(0x8A);
    lcd_writeData(0x2A);
    lcd_writeIndex(0xC4);
    lcd_writeData(0x8A);
    lcd_writeData(0xEE);

    lcd_writeIndex(0xC5);
    lcd_writeData(0x0E);

    lcd_writeIndex(0x36);

    #if(0 == LCD_DISPLAY_DIR)
        lcd_writeData(1<<7 | 1<<6 | 0<<5);
    #elif(1 == LCD_DISPLAY_DIR)
        lcd_writeData(0<<7 | 0<<6 | 0<<5);
    #elif(2 == LCD_DISPLAY_DIR)
        lcd_writeData(1<<7 | 0<<6 | 1<<5);
    #else
        lcd_writeData(0<<7 | 1<<6 | 1<<5);
    #endif

    lcd_writeIndex(0xe0);
    lcd_writeData(0x0f);
    lcd_writeData(0x1a);
    lcd_writeData(0x0f);
    lcd_writeData(0x18);
    lcd_writeData(0x2f);
    lcd_writeData(0x28);
    lcd_writeData(0x20);
    lcd_writeData(0x22);
    lcd_writeData(0x1f);
    lcd_writeData(0x1b);
    lcd_writeData(0x23);
    lcd_writeData(0x37);
    lcd_writeData(0x00);
    lcd_writeData(0x07);
    lcd_writeData(0x02);
    lcd_writeData(0x10);

    lcd_writeIndex(0xe1);
    lcd_writeData(0x0f);
    lcd_writeData(0x1b);
    lcd_writeData(0x0f);
    lcd_writeData(0x17);
    lcd_writeData(0x33);
    lcd_writeData(0x2c);
    lcd_writeData(0x29);
    lcd_writeData(0x2e);
    lcd_writeData(0x30);
    lcd_writeData(0x30);
    lcd_writeData(0x39);
    lcd_writeData(0x3f);
    lcd_writeData(0x00);
    lcd_writeData(0x07);
    lcd_writeData(0x03);
    lcd_writeData(0x10);

    lcd_writeIndex(0x2a);
    lcd_writeData(0x00);
    lcd_writeData(0x00+2);
    lcd_writeData(0x00);
    lcd_writeData(0x80+2);

    lcd_writeIndex(0x2b);
    lcd_writeData(0x00);
    lcd_writeData(0x00+3);
    lcd_writeData(0x00);
    lcd_writeData(0x80+3);

    lcd_writeIndex(0xF0);
    lcd_writeData(0x01);
    lcd_writeIndex(0xF6);
    lcd_writeData(0x00);

    lcd_writeIndex(0x3A);
    lcd_writeData(0x05);
    lcd_writeIndex(0x29);

    lcd_clear(WHITE);//全白

    LCD_BLK_HIGH;

    lcd_init_ok = 1;

    return RT_EOK;
}
INIT_DEVICE_EXPORT(lcd_init);

#define ST7735_CASET 0x2A  ///< Column Address Set
#define ST7735_PASET 0x2B  ///< Page Address Set
#define ST7735_RAMWR 0x2C  ///< Memory Write

#if(LCD_DISPLAY_DIR==0 || LCD_DISPLAY_DIR==1) //竖屏
    #define Column_offset 2
    #define Row_offset    1
#elif(LCD_DISPLAY_DIR==2 || LCD_DISPLAY_DIR==3)
    #define Column_offset 1
    #define Row_offset    2
#endif
/**
 * @brief lcd set address function(设置显示边界)
 *
 * @param x1: start column
 * @param y1: start row
 * @param x2: end column
 * @param y2: end row
 */
void lcd_set_address(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    uint16_t cmd1, cmd2;
    uint8_t data_buf[4];

    cmd1 = x1 + Column_offset;
    cmd2 = x2 + Column_offset;

    data_buf[0] = (uint8_t)(cmd1 >> 8);
    data_buf[1] = (uint8_t)(cmd1 & 0xFF);
    data_buf[2] = (uint8_t)(cmd2 >> 8);
    data_buf[3] = (uint8_t)(cmd2 & 0xFF);
    lcd_send_command_data(ST7735_CASET, data_buf, 4); // Column address set

    cmd1 = y1 + Row_offset;
    cmd2 = y2 + Row_offset;

    data_buf[0] = (uint8_t)(cmd1 >> 8);
    data_buf[1] = (uint8_t)(cmd1 & 0xFF);
    data_buf[2] = (uint8_t)(cmd2 >> 8);
    data_buf[3] = (uint8_t)(cmd2 & 0xFF);
    lcd_send_command_data(ST7735_PASET, data_buf, 4); // Row address set
    lcd_send_command_data(ST7735_RAMWR, RT_NULL, 0);  // Write to RAM
}

/**
 * @brief  lcd fill function(填充)
 *
 * @param xsta
 * @param ysta
 * @param xend
 * @param yend
 * @param color
 */
void lcd_fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color)
{
    uint16_t i, j;
    uint8_t data_buf[2];

    data_buf[0] = color >> 8;
    data_buf[1] = (uint8_t)(color & 0xFF);

    lcd_set_address(xsta, ysta, xend - 1, yend - 1);
    for (i = ysta; i < yend; i++)
          for (j = xsta; j < xend; j++)
              lcd_send_data(data_buf, 2);
}

/**
 * @brief  lcd clear function(清屏)
 *
 * @param color
 */
void lcd_clear(uint16_t color)
{
    lcd_fill(0, 0, LCD_TFTWIDTH, LCD_TFTHEIGHT, color);
}

/**
 * @brief  lcd draw pixel function(画点)
 *
 * @param x
 * @param y
 * @param color
 */
void lcd_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    uint8_t data_buf[2];

    data_buf[0] = color >> 8;
    data_buf[1] = (uint8_t)(color & 0xFF);

    lcd_set_address(x, y, x, y);
    lcd_send_data(data_buf, 2);
}

/**
 * @brief  lcd draw line function(画线)
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param color
 */
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1;
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;
    if (delta_x > 0)
        incx = 1;
    else if (delta_x == 0)
        incx = 0;
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }
    if (delta_y > 0)
        incy = 1;
    else if (delta_y == 0)
        incy = 0; //????
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    if (delta_x > delta_y)
        distance = delta_x;
    else
        distance = delta_y;
    for (t = 0; t < distance + 1; t++)
    {
        lcd_draw_pixel(uRow, uCol, color);
        xerr += delta_x;
        yerr += delta_y;
        if (xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }
        if (yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

/**
 * @brief  lcd draw rect function(画矩形)
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param color
 */
void lcd_draw_rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color)
{
    lcd_draw_line(x1, y1, x2, y1, color);
    lcd_draw_line(x1, y1, x1, y2, color);
    lcd_draw_line(x1, y2, x2, y2, color);
    lcd_draw_line(x2, y1, x2, y2, color);
}

/**
 * @brief  lcd draw circle function(画圆)
 *
 * @param x0
 * @param y0
 * @param r
 * @param color
 */
void lcd_draw_circ(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
    int a, b;
    a = 0;
    b = r;
    while (a <= b)
    {
        lcd_draw_pixel(x0 - b, y0 - a, color); // 3
        lcd_draw_pixel(x0 + b, y0 - a, color); // 0
        lcd_draw_pixel(x0 - a, y0 + b, color); // 1
        lcd_draw_pixel(x0 - a, y0 - b, color); // 2
        lcd_draw_pixel(x0 + b, y0 + a, color); // 4
        lcd_draw_pixel(x0 + a, y0 - b, color); // 5
        lcd_draw_pixel(x0 + a, y0 + b, color); // 6
        lcd_draw_pixel(x0 - b, y0 + a, color); // 7
        a++;
        if ((a * a + b * b) > (r * r))
        {
            b--;
        }
    }
}

/**
 * @brief  lcd show char function(显示字符)
 *
 * @param x
 * @param y
 * @param dat
 */
void lcd_showchar(uint16_t x, uint16_t y, uint8_t dat)
{
    uint8_t temp, data_buf[2];

    for (uint8_t i = 0; i < 16; ++i)
    {
        lcd_set_address(x, y + i, x + 7, y + i);

        temp = tft_ascii[dat - 32][i];
        for (uint8_t j = 0; j < 8; ++j)
        {
            if (temp & 0x01)
            {
                data_buf[0] = PEN_COLOR >> 8;
                data_buf[1] = (uint8_t)(PEN_COLOR & 0xFF);
                lcd_send_data(data_buf, 2);
            }
            else
            {
                data_buf[0] = BG_COLOR >> 8;
                data_buf[1] = (uint8_t)(BG_COLOR & 0xFF);
                lcd_send_data(data_buf, 2);
            }
            temp >>= 1;
        }
    }
}

/**
 * @brief  lcd show string function(显示字符串)
 *
 * @param x
 * @param y
 * @param dat
 */
void lcd_showstr(uint16_t x, uint16_t y, uint8_t dat[])
{
    uint16_t j = 0;

    while (dat[j] != '\0')
    {
        lcd_showchar(x + 8 * j, y * 16, dat[j]);
        ++j;
    }
}

/**
 * @brief 液晶显示8位有符号
 *
 * @param x 坐标x方向的起点 参数范围 0 -（TFT_X_MAX-1）
 * @param y 坐标y方向的起点 参数范围 0 -（TFT_Y_MAX/16-1）
 * @param dat 需要显示的变量，数据类型int8
 */
void lcd_showint8(uint16_t x, uint16_t y, int8_t dat)
{
    uint8_t a[3];
    uint8_t i;
    if (dat < 0)
    {
        lcd_showchar(x, y * 16, '-');
        dat = -dat;
    }
    else
    {
        lcd_showchar(x, y * 16, ' ');
    }

    a[0] = dat / 100;
    a[1] = dat / 10 % 10;
    a[2] = dat % 10;
    i = 0;
    while (i < 3)
    {
        lcd_showchar((uint8_t)(x + (8 * (i + 1))), (uint8_t)(y * 16), (uint8_t)('0' + a[i]));
        i++;
    }
}

/**
 * @brief 液晶显示8位无符号
 *
 * @param x 坐标x方向的起点 参数范围 0 -（TFT_X_MAX-1）
 * @param y 坐标y方向的起点 参数范围 0 -（TFT_Y_MAX/16-1）
 * @param dat 需要显示的变量，数据类型uint8_t
 */
void lcd_showuint8(uint16_t x,uint16_t y,uint8_t dat)
{
    uint8_t a[3];
    uint8_t i;
    a[0] = dat/100;
    a[1] = dat/10%10;
    a[2] = dat%10;
    i = 0;
    while(i<3)
    {
        lcd_showchar((uint8_t)(x+(8*(i+1))), (uint8_t)(y*16),(int8_t)('0' + a[i]));
        i++;
    }
}

/**
 * @brief 液晶显示16位有符号
 *
 * @param x 坐标x方向的起点 参数范围 0 -（TFT_X_MAX-1）
 * @param y 坐标y方向的起点 参数范围 0 -（TFT_Y_MAX/16-1）
 * @param dat 需要显示的变量，数据类型int16_t
 */
void lcd_showint16(uint16_t x,uint16_t y,int16_t dat)
{
    uint8_t a[5];
    uint8_t i;
    if(dat<0)
    {
        lcd_showchar(x,y*16,'-');
        dat = -dat;
    }
    else    lcd_showchar(x,y*16,' ');

    a[0] = dat/10000;
    a[1] = dat/1000%10;
    a[2] = dat/100%10;
    a[3] = dat/10%10;
    a[4] = dat%10;

    i = 0;
    while(i<5)
    {
        lcd_showchar((uint8_t)(x+(8*(i+1))), (uint8_t)(y*16),(int8_t)('0' + a[i]));
        i++;
    }
}

/**
 * @brief 液晶显示16位无符号
 *
 * @param x 坐标x方向的起点 参数范围 0 -（TFT_X_MAX-1）
 * @param y 坐标y方向的起点 参数范围 0 -（TFT_Y_MAX/16-1）
 * @param dat 需要显示的变量，数据类型uint16_t
 */
void lcd_showuint16(uint16_t x,uint16_t y,uint16_t dat)
{
    uint8_t a[5];
    uint8_t i;
    a[0] = dat/10000;
    a[1] = dat/1000%10;
    a[2] = dat/100%10;
    a[3] = dat/10%10;
    a[4] = dat%10;

    i = 0;
    while(i<5)
    {
        lcd_showchar((uint8_t)(x+(8*(i+1))), (uint8_t)(y*16),(int8_t)('0' + a[i]));
        i++;
    }
}

/**
 * @brief 液晶显示32位有符号(去除整数部分无效的0)
 *
 * @param x 坐标x方向的起点 参数范围 0 -（TFT_X_MAX-1）
 * @param y 坐标y方向的起点 参数范围 0 -（TFT_Y_MAX/16-1）
 * @param dat 需要显示的变量，数据类型uint32_t
 * @param num 需要显示的位数 最高10位  不包含正负号
 */
void lcd_showint32(uint16_t x, uint16_t y, int32_t dat, uint8_t num){
    uint8_t buff[34];
    uint8_t length = 0;
    uint8_t i = 0;
    int32_t temp_dat = dat;

    if (num > 10) num = 10;

    // Handle negative numbers
    if (dat < 0)
    {
        buff[0] = '-';
        length++;
        temp_dat = -dat;
    }
    else
    {
        buff[0] = ' ';
        length++;
    }

    // Convert the integer to a string
    do
    {
        buff[length++] = (temp_dat % 10) + '0';
        temp_dat /= 10;
    } while (temp_dat != 0);

    // Reverse the string excluding the sign character
    for (i = 1; i < length / 2; i++)
    {
        char temp = buff[i];
        buff[i] = buff[length - i];
        buff[length - i] = temp;
    }

    // Pad with spaces if necessary
    while (length < num + 1)
    {
        buff[length++] = ' ';
    }

    buff[length] = '\0';

    // Display the string
    lcd_showstr(x, y, buff);
}

/**
 * @brief 液晶显示浮点数(去除整数部分无效的0)
 *
 * @param x 坐标x方向的起点 参数范围 0 -（TFT_X_MAX-1）
 * @param y 坐标y方向的起点 参数范围 0 -（TFT_Y_MAX/16-1）
 * @param dat 需要显示的变量，数据类型float或double
 * @param num 整数位显示长度 最高10位
 * @param pointnum 小数位显示长度 最高6位 (当显示6位小数的时候，最后一位会不准)
 *
 * @note 特别注意当发现小数部分显示的值与你写入的值不一样的时候，
 *       可能是由于浮点数精度丢失问题导致的，这并不是显示函数的问题，
 *       有关问题的详情，请自行百度学习 浮点数精度丢失问题。
 *       负数会显示一个 ‘-’号 正数显示一个空格
 */
void lcd_showfloat(uint16_t x, uint16_t y, double dat, uint8_t num, uint8_t pointnum){
    uint8_t buff[34];
    int8_t start, end;

    if (pointnum > 6)
    {
        pointnum = 6;
    }
    if (num > 10)
    {
        num = 10;
    }

    // 手动处理浮点数的字符串表示
    bool is_negative = dat < 0;
    double abs_dat = is_negative ? -dat : dat;
    int32_t int_part = (int32_t)abs_dat;
    double frac_part = abs_dat - int_part;

    // 处理整数部分
    int32_t temp = int_part;
    int8_t int_length = 0;
    do
    {
        int_length++;
        temp /= 10;
    } while (temp > 0);

    // 填充整数部分
    temp = int_part;
    for (int8_t i = int_length - 1; i >= 0; i--)
    {
        buff[i + (is_negative ? 1 : 0)] = '0' + (temp % 10);
        temp /= 10;
    }

    // 添加小数点
    buff[int_length + (is_negative ? 1 : 0)] = '.';

    // 处理小数部分
    int8_t frac_length = 0;
    while (frac_length < pointnum)
    {
        frac_part *= 10;
        int32_t digit = (int32_t)frac_part;
        buff[int_length + 1 + frac_length + (is_negative ? 1 : 0)] = '0' + digit;
        frac_part -= digit;
        frac_length++;
    }

    // 处理负号
    if (is_negative)
    {
        buff[0] = '-';
        start = 1;
    }
    else
    {
        start = 0;
    }

    // 计算结束位
    end = int_length + 1 + pointnum + (is_negative ? 1 : 0);

    // 填充空格
    while (int_length < num)
    {
        buff[end] = ' ';
        end++;
        int_length++;
    }

    buff[end] = '\0';

    lcd_showstr(x, y, buff); // 显示数字
}

// 显示汉字的函数
void lcd_show_chinese(uint16_t x, uint16_t y, uint8_t index, uint16_t color) {
    const uint8_t *font_data = my_hz_font[2*index];
    uint8_t data_buf[2];

    for (uint8_t i = 0; i < 16; ++i) {
        lcd_set_address(x, y + i, x + 15, y + i);

        for (uint8_t j = 0; j < 2; ++j) {
            uint8_t temp = font_data[i * 2 + j];
            for (uint8_t k = 0; k < 8; ++k) {
                if (temp & 0x01) {
                    data_buf[0] = color >> 8;
                    data_buf[1] = (uint8_t)(color & 0xFF);
                    lcd_send_data(data_buf, 2);
                } else {
                    data_buf[0] = 0xFF; // 背景色为白色
                    data_buf[1] = 0xFF;
                    lcd_send_data(data_buf, 2);
                }
                temp >>= 1;
            }
        }
    }
}

/**
 * @brief  lcd show picture function(显示图片)
 *
 * @param x
 * @param y
 * @param width
 * @param hight
 * @param picture
 */
void lcd_showpicture(uint16_t x,uint16_t y,uint16_t width,uint16_t hight,const uint8_t picture[])
{
    uint32_t k=0;

    lcd_set_address(x, y, x + width - 1, y + hight - 1);

    for(uint16_t i = 0;i < width; ++i)
    {
        for(uint16_t j = 0; j < hight; ++j)
        {
            lcd_send_data((uint8_t *)&picture[k * 2], 1);
            lcd_send_data((uint8_t *)&picture[k * 2 + 1], 1);
            ++k;
        }
    }
}

/***********************************************/
#ifdef BSP_USING_LCD
#define LCD_DEVICE(dev)     (struct drv_lcd_device*)(dev)

static LTDC_HandleTypeDef LtdcHandle = {0};

struct drv_lcd_device
{
    struct rt_device parent;

    struct rt_device_graphic_info lcd_info;

    struct rt_semaphore lcd_lock;

    /* 0:front_buf is being used 1: back_buf is being used*/
    rt_uint8_t cur_buf;
    rt_uint8_t *front_buf;
    rt_uint8_t *back_buf;
};

struct drv_lcd_device _lcd;

static rt_err_t drv_lcd_init(struct rt_device *device)
{
    struct drv_lcd_device *lcd = LCD_DEVICE(device);
    /* nothing, right now */
    lcd = lcd;
    return RT_EOK;
}

static rt_err_t drv_lcd_control(struct rt_device *device, int cmd, void *args)
{
    struct drv_lcd_device *lcd = LCD_DEVICE(device);

    switch (cmd)
    {
    case RTGRAPHIC_CTRL_RECT_UPDATE:
    {
        /* update */
        if (_lcd.cur_buf)
        {
            /* back_buf is being used */
            memcpy(_lcd.front_buf, _lcd.lcd_info.framebuffer, LCD_BUF_SIZE);
            /* Configure the color frame buffer start address */
            LTDC_LAYER(&LtdcHandle, 0)->CFBAR &= ~(LTDC_LxCFBAR_CFBADD);
            LTDC_LAYER(&LtdcHandle, 0)->CFBAR = (uint32_t)(_lcd.front_buf);
            _lcd.cur_buf = 0;
        }
        else
        {
            /* front_buf is being used */
            memcpy(_lcd.back_buf, _lcd.lcd_info.framebuffer, LCD_BUF_SIZE);
            /* Configure the color frame buffer start address */
            LTDC_LAYER(&LtdcHandle, 0)->CFBAR &= ~(LTDC_LxCFBAR_CFBADD);
            LTDC_LAYER(&LtdcHandle, 0)->CFBAR = (uint32_t)(_lcd.back_buf);
            _lcd.cur_buf = 1;
        }
        rt_sem_take(&_lcd.lcd_lock, RT_TICK_PER_SECOND / 20);
        HAL_LTDC_Relaod(&LtdcHandle, LTDC_SRCR_VBR);
    }
    break;

    case RTGRAPHIC_CTRL_GET_INFO:
    {
        struct rt_device_graphic_info *info = (struct rt_device_graphic_info *)args;

        RT_ASSERT(info != RT_NULL);
        info->pixel_format  = lcd->lcd_info.pixel_format;
        info->bits_per_pixel = 16;
        info->width         = lcd->lcd_info.width;
        info->height        = lcd->lcd_info.height;
        info->framebuffer   = lcd->lcd_info.framebuffer;
    }
    break;

    default:
        return -RT_EINVAL;
    }

    return RT_EOK;
}

void HAL_LTDC_ReloadEventCallback(LTDC_HandleTypeDef *hltdc)
{
    /* emable line interupt */
    __HAL_LTDC_ENABLE_IT(&LtdcHandle, LTDC_IER_LIE);
}

void HAL_LTDC_LineEventCallback(LTDC_HandleTypeDef *hltdc)
{
    rt_sem_release(&_lcd.lcd_lock);
}

void LTDC_IRQHandler(void)
{
    rt_interrupt_enter();

    HAL_LTDC_IRQHandler(&LtdcHandle);

    rt_interrupt_leave();
}

rt_err_t stm32_lcd_init(struct drv_lcd_device *lcd)
{
    LTDC_LayerCfgTypeDef pLayerCfg = {0};

    /* LTDC Initialization -------------------------------------------------------*/

    /* Polarity configuration */
    /* Initialize the horizontal synchronization polarity as active low */
    LtdcHandle.Init.HSPolarity = LTDC_HSPOLARITY_AL;
    /* Initialize the vertical synchronization polarity as active low */
    LtdcHandle.Init.VSPolarity = LTDC_VSPOLARITY_AL;
    /* Initialize the data enable polarity as active low */
    LtdcHandle.Init.DEPolarity = LTDC_DEPOLARITY_AL;
    /* Initialize the pixel clock polarity as input pixel clock */
    LtdcHandle.Init.PCPolarity = LTDC_PCPOLARITY_IPC;

    /* Timing configuration */
    /* Horizontal synchronization width = Hsync - 1 */
    LtdcHandle.Init.HorizontalSync = LCD_HSYNC_WIDTH - 1;
    /* Vertical synchronization height = Vsync - 1 */
    LtdcHandle.Init.VerticalSync = LCD_VSYNC_HEIGHT - 1;
    /* Accumulated horizontal back porch = Hsync + HBP - 1 */
    LtdcHandle.Init.AccumulatedHBP = LCD_HSYNC_WIDTH + LCD_HBP - 1;
    /* Accumulated vertical back porch = Vsync + VBP - 1 */
    LtdcHandle.Init.AccumulatedVBP = LCD_VSYNC_HEIGHT + LCD_VBP - 1;
    /* Accumulated active width = Hsync + HBP + Active Width - 1 */
    LtdcHandle.Init.AccumulatedActiveW = LCD_HSYNC_WIDTH + LCD_HBP + lcd->lcd_info.width - 1 ;
    /* Accumulated active height = Vsync + VBP + Active Heigh - 1 */
    LtdcHandle.Init.AccumulatedActiveH = LCD_VSYNC_HEIGHT + LCD_VBP + lcd->lcd_info.height - 1;
    /* Total height = Vsync + VBP + Active Heigh + VFP - 1 */
    LtdcHandle.Init.TotalHeigh = LtdcHandle.Init.AccumulatedActiveH + LCD_VFP;
    /* Total width = Hsync + HBP + Active Width + HFP - 1 */
    LtdcHandle.Init.TotalWidth = LtdcHandle.Init.AccumulatedActiveW + LCD_HFP;

    /* Configure R,G,B component values for LCD background color */
    LtdcHandle.Init.Backcolor.Blue = 0;
    LtdcHandle.Init.Backcolor.Green = 0;
    LtdcHandle.Init.Backcolor.Red = 0;

    LtdcHandle.Instance = LTDC;

    /* Layer1 Configuration ------------------------------------------------------*/

    /* Windowing configuration */
    pLayerCfg.WindowX0 = 0;
    pLayerCfg.WindowX1 = lcd->lcd_info.width;
    pLayerCfg.WindowY0 = 0;
    pLayerCfg.WindowY1 = lcd->lcd_info.height;

    /* Pixel Format configuration*/
    if (lcd->lcd_info.pixel_format == RTGRAPHIC_PIXEL_FORMAT_RGB565)
    {
        pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
    }
    else if (lcd->lcd_info.pixel_format == RTGRAPHIC_PIXEL_FORMAT_ARGB888)
    {
        pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
    }
    else if (lcd->lcd_info.pixel_format == RTGRAPHIC_PIXEL_FORMAT_RGB888)
    {
        pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB888;
    }
    else if (lcd->lcd_info.pixel_format == RTGRAPHIC_PIXEL_FORMAT_RGB888)
    {
        pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB888;
    }
    else
    {
        LOG_E("unsupported pixel format");
        return -RT_ERROR;
    }

    /* Start Address configuration : frame buffer is located at FLASH memory */
    pLayerCfg.FBStartAdress = (uint32_t)lcd->front_buf;

    /* Alpha constant (255 totally opaque) */
    pLayerCfg.Alpha = 255;

    /* Default Color configuration (configure A,R,G,B component values) */
    pLayerCfg.Alpha0 = 255;
    pLayerCfg.Backcolor.Blue = 0;
    pLayerCfg.Backcolor.Green = 0;
    pLayerCfg.Backcolor.Red = 0;

    /* Configure blending factors */
    /* Constant Alpha value:  pLayerCfg.Alpha / 255
       C: Current Layer Color
       Cs: Background color
       BC = Constant Alpha x C + (1 - Constant Alpha ) x Cs */
    /* BlendingFactor1: Pixel Alpha x Constant Alpha */
    pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
    /* BlendingFactor2: 1 - (Pixel Alpha x Constant Alpha) */
    pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;

    /* Configure the number of lines and number of pixels per line */
    pLayerCfg.ImageWidth = lcd->lcd_info.width;
    pLayerCfg.ImageHeight = lcd->lcd_info.height;

    /* Configure the LTDC */
    if (HAL_LTDC_Init(&LtdcHandle) != HAL_OK)
    {
        LOG_E("LTDC init failed");
        return -RT_ERROR;
    }

    /* Configure the Background Layer*/
    if (HAL_LTDC_ConfigLayer(&LtdcHandle, &pLayerCfg, 0) != HAL_OK)
    {
        LOG_E("LTDC layer init failed");
        return -RT_ERROR;
    }
    else
    {
        /* enable LTDC interrupt */
        HAL_NVIC_SetPriority(LTDC_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(LTDC_IRQn);
        LOG_D("LTDC init success");
        return RT_EOK;
    }
}
#if defined(LCD_BACKLIGHT_USING_PWM)
void turn_on_lcd_backlight(void)
{
    struct rt_device_pwm *pwm_dev;

    /* turn on the LCD backlight */
    pwm_dev = (struct rt_device_pwm *)rt_device_find(LCD_PWM_DEV_NAME);
    /* pwm frequency:100K = 10000ns */
    rt_pwm_set(pwm_dev, LCD_PWM_DEV_CHANNEL, 10000, 10000);
    rt_pwm_enable(pwm_dev, LCD_PWM_DEV_CHANNEL);
}
#elif defined(LCD_BACKLIGHT_USING_GPIO)
void turn_on_lcd_backlight(void)
{
    rt_pin_mode(LCD_BL_GPIO_NUM, PIN_MODE_OUTPUT);
    rt_pin_mode(LCD_DISP_GPIO_NUM, PIN_MODE_OUTPUT);

    rt_pin_write(LCD_DISP_GPIO_NUM, PIN_HIGH);
    rt_pin_write(LCD_BL_GPIO_NUM, PIN_HIGH);
}
#else
void turn_on_lcd_backlight(void)
{

}
#endif

#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops lcd_ops =
{
    drv_lcd_init,
    RT_NULL,
    RT_NULL,
    RT_NULL,
    RT_NULL,
    drv_lcd_control
};
#endif

int drv_lcd_hw_init(void)
{
    rt_err_t result = RT_EOK;
    struct rt_device *device = &_lcd.parent;

    /* memset _lcd to zero */
    memset(&_lcd, 0x00, sizeof(_lcd));

    /* init lcd_lock semaphore */
    result = rt_sem_init(&_lcd.lcd_lock, "lcd_lock", 0, RT_IPC_FLAG_FIFO);
    if (result != RT_EOK)
    {
        LOG_E("init semaphore failed!\n");
        result = -RT_ENOMEM;
        goto __exit;
    }

    /* config LCD dev info */
    _lcd.lcd_info.height = LCD_HEIGHT;
    _lcd.lcd_info.width = LCD_WIDTH;
    _lcd.lcd_info.bits_per_pixel = LCD_BITS_PER_PIXEL;
    _lcd.lcd_info.pixel_format = LCD_PIXEL_FORMAT;

    /* malloc memory for Triple Buffering */
    _lcd.lcd_info.framebuffer = rt_malloc(LCD_BUF_SIZE);
    _lcd.back_buf = rt_malloc(LCD_BUF_SIZE);
    _lcd.front_buf = rt_malloc(LCD_BUF_SIZE);
    if (_lcd.lcd_info.framebuffer == RT_NULL || _lcd.back_buf == RT_NULL || _lcd.front_buf == RT_NULL)
    {
        LOG_E("init frame buffer failed!\n");
        result = -RT_ENOMEM;
        goto __exit;
    }

    /* memset buff to 0xFF */
    memset(_lcd.lcd_info.framebuffer, 0xFF, LCD_BUF_SIZE);
    memset(_lcd.back_buf, 0xFF, LCD_BUF_SIZE);
    memset(_lcd.front_buf, 0xFF, LCD_BUF_SIZE);

    device->type    = RT_Device_Class_Graphic;
#ifdef RT_USING_DEVICE_OPS
    device->ops     = &lcd_ops;
#else
    device->init    = drv_lcd_init;
    device->control = drv_lcd_control;
#endif

    /* register lcd device */
    rt_device_register(device, "lcd", RT_DEVICE_FLAG_RDWR);

    /* init stm32 LTDC */
    if (stm32_lcd_init(&_lcd) != RT_EOK)
    {
        result = -RT_ERROR;
        goto __exit;
    }
    else
    {
        turn_on_lcd_backlight();
    }

__exit:
    if (result != RT_EOK)
    {
        rt_sem_detach(&_lcd.lcd_lock);

        if (_lcd.lcd_info.framebuffer)
        {
            rt_free(_lcd.lcd_info.framebuffer);
        }

        if (_lcd.back_buf)
        {
            rt_free(_lcd.back_buf);
        }

        if (_lcd.front_buf)
        {
            rt_free(_lcd.front_buf);
        }
    }
    return result;
}
INIT_DEVICE_EXPORT(drv_lcd_hw_init);

#ifdef DRV_DEBUG
#ifdef FINSH_USING_MSH
int lcd_test()
{
    struct drv_lcd_device *lcd;
    lcd = (struct drv_lcd_device *)rt_device_find("lcd");

    while (1)
    {
        /* red */
        for (int i = 0; i < LCD_BUF_SIZE / 2; i++)
        {
            lcd->lcd_info.framebuffer[2 * i] = 0x00;
            lcd->lcd_info.framebuffer[2 * i + 1] = 0xF8;
        }
        lcd->parent.control(&lcd->parent, RTGRAPHIC_CTRL_RECT_UPDATE, RT_NULL);
        rt_thread_mdelay(1000);
        /* green */
        for (int i = 0; i < LCD_BUF_SIZE / 2; i++)
        {
            lcd->lcd_info.framebuffer[2 * i] = 0xE0;
            lcd->lcd_info.framebuffer[2 * i + 1] = 0x07;
        }
        lcd->parent.control(&lcd->parent, RTGRAPHIC_CTRL_RECT_UPDATE, RT_NULL);
        rt_thread_mdelay(1000);
        /* blue */
        for (int i = 0; i < LCD_BUF_SIZE / 2; i++)
        {
            lcd->lcd_info.framebuffer[2 * i] = 0x1F;
            lcd->lcd_info.framebuffer[2 * i + 1] = 0x00;
        }
        lcd->parent.control(&lcd->parent, RTGRAPHIC_CTRL_RECT_UPDATE, RT_NULL);
        rt_thread_mdelay(1000);
    }
}
MSH_CMD_EXPORT(lcd_test, lcd_test);
#endif /* FINSH_USING_MSH */
#endif /* DRV_DEBUG */
#endif /* BSP_USING_LCD */
