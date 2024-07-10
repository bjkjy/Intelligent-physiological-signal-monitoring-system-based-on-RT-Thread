/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-07-06     Baijiankang       the first version
 */
#ifndef APPLICATIONS_UART_H_
#define APPLICATIONS_UART_H_

/*#define USING_SERIAL_UART2*/
#define USING_BLUETOOTH_UART2
/*#define USING_SERIAL_UART3*/
/*#define USING_BLUETOOTH_UART3*/

typedef enum
{
    NUMBER = 0,
    STRING,
    NULL_TYPE,
    ERROR_TYPE
}recv_type_t;

extern rt_device_t uart2_dev, uart3_dev;

extern rt_uint8_t uart_init_finished;

extern recv_type_t g_type_of_recv_uart2;
extern rt_uint8_t g_received_finished_uart2;
extern float g_received_float_uart2;
extern char g_received_string_uart2[20];

extern recv_type_t g_type_of_recv_uart3;
extern rt_uint8_t g_received_finished_uart3;
extern float g_received_float_uart3;
extern char g_received_string_uart3[20];

int uart_putchar(rt_device_t uartx_dev,char dat);
char uart_getchar(rt_device_t uartx_dev);
int uart_putstring(rt_device_t uartx_dev, const char *str);

#endif /* APPLICATIONS_UART_H_ */
