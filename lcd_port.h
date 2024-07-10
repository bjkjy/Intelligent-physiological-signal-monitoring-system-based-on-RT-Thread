/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-06-27     Baijiankang       the first version
 */
#ifndef DRIVERS_INCLUDE_LCD_PORT_H_
#define DRIVERS_INCLUDE_LCD_PORT_H_

#define LCD_CS_PORT GPIOB
#define LCD_CS_PIN GPIO_PIN_9

#define LCD_CS_GET_PIN      GET_PIN(B,9)
#define LCD_DC_GET_PIN      GET_PIN(B,12)
#define LCD_RST_GET_PIN     GET_PIN(B,14)
#define LCD_BLK_GET_PIN     GET_PIN(B,1)

#define LCD_SPI_BUS "spi2"
#define LCD_SPI_DEV_NAME "spi20"

#define LCD_CS_HIGH rt_pin_write(LCD_CS_GET_PIN, PIN_HIGH);
#define LCD_CS_LOW  rt_pin_write(LCD_CS_GET_PIN, PIN_LOW);

#define LCD_DC_HIGH rt_pin_write(LCD_DC_GET_PIN, PIN_HIGH);
#define LCD_DC_LOW  rt_pin_write(LCD_DC_GET_PIN, PIN_LOW);

#define LCD_RST_HIGH rt_pin_write(LCD_RST_GET_PIN, PIN_HIGH);
#define LCD_RST_LOW  rt_pin_write(LCD_RST_GET_PIN, PIN_LOW);

#define LCD_BLK_HIGH rt_pin_write(LCD_BLK_GET_PIN, PIN_HIGH);
#define LCD_BLK_LOW  rt_pin_write(LCD_BLK_GET_PIN, PIN_LOW);

#endif /* DRIVERS_INCLUDE_LCD_PORT_H_ */
