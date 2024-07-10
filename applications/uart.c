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
#include <board.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include "uart.h"
#include "user.h"

rt_device_t uart2_dev, uart3_dev;

struct serial_configure uart2_config = RT_SERIAL_CONFIG_DEFAULT, uart3_config = RT_SERIAL_CONFIG_DEFAULT;

rt_int8_t uart2_recv_buffer[20] = {0}, uart3_recv_buffer[20] = {0};
rt_int8_t uart2_index0, uart3_index0; //索引 index已定义

recv_type_t g_type_of_recv_uart2 = NULL_TYPE;
rt_uint8_t g_received_finished_uart2 = 0;
float g_received_float_uart2 = 0.0f;
char g_received_string_uart2[20] = {0};

recv_type_t g_type_of_recv_uart3 = NULL_TYPE;
rt_uint8_t g_received_finished_uart3 = 0;
float g_received_float_uart3 = 0.0f;
char g_received_string_uart3[20] = {0};

char *returns_error_style = "Please Enter The Correct Format!\n";
char *returns_null = "You didn't Enter Anything. Please Enter Something!\n";

rt_uint8_t uart_init_finished = 0;
/**
 * @brief user_uart2_callback 用户交互
 * 
 */
void user_uart2_callback(void)
{
    if(g_type_of_recv_uart2 == STRING)
    {
        if(strcmp((char*)uart2_recv_buffer, "HR") == 0)
        {
            uart_putstring(uart2_dev, "Heart Rate: ");
            char hr_str[10];
            sprintf(hr_str, "%d", (rt_int16_t)blood_result.HR);
            uart_putstring(uart2_dev, hr_str);
            uart_putstring(uart2_dev, "\n");
        }
        else if(strcmp((char*)uart2_recv_buffer, "SpO2") == 0)
        {
            uart_putstring(uart2_dev, "SpO2: ");
            char spo2_str[10];
            sprintf(spo2_str, "%d", (int16_t)blood_result.SpO2);
            uart_putstring(uart2_dev, spo2_str);
            uart_putstring(uart2_dev, "\n");
        }
        else
        {
            uart_putstring(uart2_dev, "Please Enter The Correct Command!\n");
        }
    }
    //rt_kprintf("uart2 received finished, the data type: %d\n", g_type_of_recv_uart2);
}

/**
 * @brief user_uart3_callback 用户交互
 * 
 */
void user_uart3_callback(void)
{
    rt_kprintf("uart3 received finished, the data type: %d\n", g_type_of_recv_uart3);
}

/**
 * @brief 通过UART设备发送一个字符
 *
 * @param uartx_dev UART设备句柄
 * @param dat 要发送的字符(char)
 * @return int
 */
int uart_putchar(rt_device_t uartx_dev,char dat){
    if (uartx_dev == RT_NULL)
        {
            rt_set_errno(-RT_ERROR);
            return -RT_ERROR;
        }

    rt_size_t result = rt_device_write(uartx_dev, 0, &dat, 1);
    if(result != 1)
        {
            rt_set_errno(-RT_ERROR);
            return -RT_ERROR;
        }
    return RT_EOK;
}

/**
 * @brief 通过UART设备接收一个字符
 *
 * @param uartx_dev UART设备句柄
 * @return char 返回接收到的字符，如果读取失败则返回-1
 */
char uart_getchar(rt_device_t uartx_dev){
    if (uartx_dev == RT_NULL)
    {
        rt_set_errno(-RT_ERROR);
        return -1;
    }

    char dat_recv = 0;
    rt_size_t result = rt_device_read(uartx_dev, 0, &dat_recv, 1);
    if (result != 1)
    {
        rt_set_errno(-RT_ERROR);
        return -1;
    }

    return dat_recv;
}

/**
 * @brief 通过UART设备发送一个字符串
 *
 * @param uartx_dev UART设备句柄
 * @param str 要发送的字符串
 * @return int 返回RT_EOK表示成功，否则表示失败
 */
int uart_putstring(rt_device_t uartx_dev, const char *str){
    if (uartx_dev == RT_NULL || str == RT_NULL)
    {
        rt_set_errno(-RT_ERROR);
        return -RT_ERROR;
    }

    rt_size_t length = rt_strlen(str);
    rt_size_t written = rt_device_write(uartx_dev, 0, str, length);

    if (written != length)
    {
        rt_set_errno(-RT_ERROR);
        return -RT_ERROR;
    }

    return RT_EOK;
}

char num_str[32]; // 足够存储 int32_t 类型的数字字符串和小数部分
/**
 * @brief 从UART2设备读取数据，并根据读取的数据类型进行处理。
 * @brief 读取的数据可能是数字或字符串。数字数据被进一步处理以确定其大小和精度。
 * @brief 当接收到回车符时，根据数据类型输出相应的信息。
 * @param void 无需参数。
 * @return rt_err_t 返回操作状态，RT_EOK表示操作成功。
 */
rt_err_t uart2_function(){
    static recv_type_t type_of_recv_uart2 = NULL_TYPE;
    static rt_int32_t recv_data_B = 0; // 存储接收到的数字的整数部分
    static rt_int32_t recv_data_F = 0; // 存储接收到的数字的小数部分
    static rt_int8_t point_position = 0;
    static rt_int8_t decimal_places = 0;

    char recv_char = uart_getchar(uart2_dev);
    if (recv_char == -1)
    {
        rt_set_errno(-RT_ERROR);
        return -RT_ERROR;
    }

    //--------不是回车--------------------------------
    if (((type_of_recv_uart2 == NULL_TYPE) || (type_of_recv_uart2 == NUMBER)) && (recv_char >= '0' && recv_char <= '9')) // 0-9
    {   
        g_received_finished_uart2 = 0;
        type_of_recv_uart2 = NUMBER;
        if (point_position == 0)
        {
            recv_data_B = recv_data_B * 10 + (recv_char - '0');
        }
        else
        {
            recv_data_F = recv_data_F * 10 + (recv_char - '0');
            decimal_places++;
        }
    }
    else if (((type_of_recv_uart2 == NULL_TYPE) || (type_of_recv_uart2 == STRING)) && (recv_char != '\n'))
    {
        g_received_finished_uart2 = 0;
        type_of_recv_uart2 = STRING;
        uart2_recv_buffer[uart2_index0++] = recv_char;
    }
    else if ((recv_char != '\n') && (type_of_recv_uart2 == NUMBER) && (recv_char != '.'))
    {
        g_received_finished_uart2 = 0;
        type_of_recv_uart2 = ERROR_TYPE;
        uart2_index0 = 0;
        recv_data_B = 0;
        recv_data_F = 0;
        decimal_places = 0;
    }
    else if ((type_of_recv_uart2 == NUMBER) && (recv_char == '.'))
    {
        g_received_finished_uart2 = 0;
        point_position = 1;
    }
    //--------是回车---------------------------------
    else if (recv_char == '\n')
    {
        if (type_of_recv_uart2 == NUMBER)
        {
            //rt_device_write(uart2_dev, 0, "numbers\n", rt_strlen("numbers\n"));
            int index = 0;

            // 处理整数部分
            int temp = recv_data_B;
            if (temp < 0)
            {
                num_str[index++] = '-';
                temp = -temp;
            }
            if (temp == 0)
            {
                num_str[index++] = '0';
            }
            else
            {
                int digits[10];
                int digit_count = 0;
                while (temp > 0)
                {
                    digits[digit_count++] = temp % 10;
                    temp /= 10;
                }
                for (int i = digit_count - 1; i >= 0; i--)
                {
                    num_str[index++] = '0' + digits[i];
                }
            }

            // 处理小数部分
            if (point_position != 0)
            {
                num_str[index++] = '.';
                temp = recv_data_F;
                int digits[10];
                int digit_count = 0;
                while (temp > 0)
                {
                    digits[digit_count++] = temp % 10;
                    temp /= 10;
                }
                for (int i = digit_count - 1; i >= 0; i--)
                {
                    num_str[index++] = '0' + digits[i];
                }
            }

            num_str[index] = '\0';
            //rt_device_write(uart2_dev, 0, "data = ", rt_strlen("data = "));
            //rt_device_write(uart2_dev, 0, num_str, rt_strlen(num_str));
            //rt_device_write(uart2_dev, 0, "\n", 1);

            // 将接收到的数据存储到全局变量中
            //rt_kprintf("%d\t",recv_data_B);
            g_received_float_uart2 = recv_data_B + (recv_data_F / pow(10, decimal_places));
            //rt_kprintf("%d\n",(int)g_received_float_uart2);
            // 重置缓冲区和索引
            recv_data_B = 0;
            recv_data_F = 0;
            point_position = 0;
            decimal_places = 0;
            g_type_of_recv_uart2 = type_of_recv_uart2;
            g_received_finished_uart2 = 1;
            type_of_recv_uart2 = NULL_TYPE;
        }
        else if (type_of_recv_uart2 == STRING)
        {
            //rt_device_write(uart2_dev, 0, "strings\n", rt_strlen("strings\n"));
            uart2_recv_buffer[uart2_index0++] = '\0';
            //rt_device_write(uart2_dev, 0, uart2_recv_buffer, uart2_index0);
            //rt_device_write(uart2_dev, 0, "\n", 1);

            // 将接收到的字符串存储到全局变量中
            strncpy(g_received_string_uart2, (char*)uart2_recv_buffer, sizeof(g_received_string_uart2) - 1);

            // 重置缓冲区和索引
            uart2_index0 = 0;
            g_type_of_recv_uart2 = type_of_recv_uart2;
            g_received_finished_uart2 = 1;
            type_of_recv_uart2 = NULL_TYPE;
        }
        else if (type_of_recv_uart2 == ERROR_TYPE)
        {
            rt_device_write(uart2_dev, 0, returns_error_style, rt_strlen(returns_error_style));
            g_type_of_recv_uart2 = type_of_recv_uart2;
            g_received_finished_uart2 = 1;
            type_of_recv_uart2 = NULL_TYPE;
            recv_data_B = 0;
            recv_data_F = 0;
            point_position = 0;
            decimal_places = 0;
        }
        else // NULL_TYPE
        {
            rt_device_write(uart2_dev, 0, returns_null, rt_strlen(returns_null));
            recv_data_B = 0;
            recv_data_F = 0;
            point_position = 0;
            decimal_places = 0;
        }
    }
    if(g_received_finished_uart2)
        user_uart2_callback();
    return RT_EOK;
}

/**
 * @brief 从UART3设备读取数据，并根据读取的数据类型进行处理。
 * @brief 读取的数据可能是数字或字符串。数字数据被进一步处理以确定其大小和精度。
 * @brief 当接收到回车符时，根据数据类型输出相应的信息。
 * @param void 无需参数。
 * @return rt_err_t 返回操作状态，RT_EOK表示操作成功。
 */
rt_err_t uart3_function(){
    static recv_type_t type_of_recv_uart3 = NULL_TYPE;
    static rt_int32_t recv_data_B = 0; // 存储接收到的数字的整数部分
    static rt_int32_t recv_data_F = 0; // 存储接收到的数字的小数部分
    static rt_int8_t point_position = 0;
    static rt_int8_t decimal_places = 0;

    char recv_char = uart_getchar(uart3_dev);
    if (recv_char == -1)
    {
        rt_set_errno(-RT_ERROR);
        return -RT_ERROR;
    }

    //--------不是回车--------------------------------
    if (((type_of_recv_uart3 == NULL_TYPE) || (type_of_recv_uart3 == NUMBER)) && (recv_char >= '0' && recv_char <= '9')) // 0-9
    {
        g_received_finished_uart3 = 0;
        type_of_recv_uart3 = NUMBER;
        if (point_position == 0)
        {
            recv_data_B = recv_data_B * 10 + (recv_char - '0');
        }
        else
        {
            recv_data_F = recv_data_F * 10 + (recv_char - '0');
            decimal_places++;
        }
    }
    else if (((type_of_recv_uart3 == NULL_TYPE) || (type_of_recv_uart3 == STRING)) && (recv_char != '\n'))
    {
        g_received_finished_uart3 = 0;
        type_of_recv_uart3 = STRING;
        uart3_recv_buffer[uart3_index0++] = recv_char;
    }
    else if ((recv_char != '\n') && (type_of_recv_uart3 == NUMBER) && (recv_char != '.'))
    {
        g_received_finished_uart3 = 0;
        type_of_recv_uart3 = ERROR_TYPE;
        uart3_index0 = 0;
        recv_data_B = 0;
        recv_data_F = 0;
        decimal_places = 0;
    }
    else if ((type_of_recv_uart3 == NUMBER) && (recv_char == '.'))
    {
        g_received_finished_uart3 = 0;
        point_position = 1;
    }
    //--------是回车---------------------------------
    else if (recv_char == '\n')
    {
        if (type_of_recv_uart3 == NUMBER)
        {
            rt_device_write(uart3_dev, 0, "numbers\n", rt_strlen("numbers\n"));
            char num_str[32]; // 足够存储 int32_t 类型的数字字符串和小数部分
            int index = 0;

            // 处理整数部分
            int temp = recv_data_B;
            if (temp < 0)
            {
                num_str[index++] = '-';
                temp = -temp;
            }
            if (temp == 0)
            {
                num_str[index++] = '0';
            }
            else
            {
                int digits[10];
                int digit_count = 0;
                while (temp > 0)
                {
                    digits[digit_count++] = temp % 10;
                    temp /= 10;
                }
                for (int i = digit_count - 1; i >= 0; i--)
                {
                    num_str[index++] = '0' + digits[i];
                }
            }

            // 处理小数部分
            if (point_position != 0)
            {
                num_str[index++] = '.';
                temp = recv_data_F;
                int digits[10];
                int digit_count = 0;
                while (temp > 0)
                {
                    digits[digit_count++] = temp % 10;
                    temp /= 10;
                }
                for (int i = digit_count - 1; i >= 0; i--)
                {
                    num_str[index++] = '0' + digits[i];
                }
            }

            num_str[index] = '\0';
            rt_device_write(uart3_dev, 0, "data = ", rt_strlen("data = "));
            rt_device_write(uart3_dev, 0, num_str, rt_strlen(num_str));
            rt_device_write(uart3_dev, 0, "\n", 1);

            // 将接收到的数据存储到全局变量中
            rt_kprintf("%d\t",recv_data_B);
            g_received_float_uart3 = recv_data_B + (recv_data_F / pow(10, decimal_places));
            rt_kprintf("%d\n",(int)g_received_float_uart3);
            // 重置缓冲区和索引
            recv_data_B = 0;
            recv_data_F = 0;
            point_position = 0;
            decimal_places = 0;
            g_type_of_recv_uart3 = type_of_recv_uart3;
            g_received_finished_uart3 = 1;
            type_of_recv_uart3 = NULL_TYPE;
        }
        else if (type_of_recv_uart3 == STRING)
        {
            rt_device_write(uart3_dev, 0, "strings\n", rt_strlen("strings\n"));
            uart3_recv_buffer[uart3_index0++] = '\0';
            rt_device_write(uart3_dev, 0, uart3_recv_buffer, uart3_index0);
            rt_device_write(uart3_dev, 0, "\n", 1);

            // 将接收到的字符串存储到全局变量中
            strncpy(g_received_string_uart3, (char*)uart3_recv_buffer, sizeof(g_received_string_uart3) - 1);

            // 重置缓冲区和索引
            uart3_index0 = 0;
            g_type_of_recv_uart3 = type_of_recv_uart3;
            g_received_finished_uart3 = 1;
            type_of_recv_uart3 = NULL_TYPE;
        }
        else if (type_of_recv_uart3 == ERROR_TYPE)
        {
            rt_device_write(uart3_dev, 0, returns_error_style, rt_strlen(returns_error_style));
            g_type_of_recv_uart3 = type_of_recv_uart3;
            g_received_finished_uart3 = 1;
            type_of_recv_uart3 = NULL_TYPE;
            recv_data_B = 0;
            recv_data_F = 0;
            point_position = 0;
            decimal_places = 0;
        }
        else // NULL_TYPE
        {
            rt_device_write(uart3_dev, 0, returns_null, rt_strlen(returns_null));
            recv_data_B = 0;
            recv_data_F = 0;
            point_position = 0;
            decimal_places = 0;
        }
    }
    if(g_received_finished_uart3)
        user_uart3_callback();
    return RT_EOK;
}

/**
 * @brief 初始化UART2设备。
 * @param void 无需参数。
 * @return int 返回操作状态，RT_EOK表示初始化成功，RT_ERROR表示初始化失败。
 */
rt_int16_t init_uart2(){
    rt_int8_t result_open;

    uart2_dev = rt_device_find("uart2");
    if(uart2_dev == RT_NULL)
    {
        rt_kprintf("UART2 Search Failed!\n");
        return RT_ERROR;
    }

    result_open = rt_device_open(uart2_dev, RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    if(result_open < 0)
    {
        rt_kprintf("UART2 Open Failed!\n");
        return RT_ERROR;
    }

    else
    {
        // 配置串口参数
#ifdef USING_BLUETOOTH_UART2
        uart2_config.baud_rate = BAUD_RATE_9600;
#endif
        rt_device_control(uart2_dev, RT_DEVICE_CTRL_CONFIG, (void *)&uart2_config);
        // 设置接收指示函数
        rt_device_set_rx_indicate(uart2_dev, uart2_function);
    }

    rt_kprintf("UART2 initialized successfully!\n");

    uart_putstring(uart2_dev, "\n*****************************************\n\n\tHello!  Physiological Signal Monitoring System!\n\n*****************************************\n\n");

    // uart_putstring(uart2_dev, init_welcome_msg);
    uart_init_finished = 1;

    return RT_EOK;
}
// INIT_DEVICE_EXPORT(init_uart2);


/**
 * @brief 初始化UART3设备。
 * @param void 无需参数。
 * @return int 返回操作状态，RT_EOK表示初始化成功，RT_ERROR表示初始化失败。
 */
int init_uart3(){
    rt_int8_t result_open;

    uart3_dev = rt_device_find("uart3");
    if(uart3_dev == RT_NULL)
    {
        rt_kprintf("UART3 Search Failed!\n");
        return RT_ERROR;
    }

    result_open = rt_device_open(uart3_dev, RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    if(result_open < 0)
    {
        rt_kprintf("UART3 Open Failed!\n");
        return RT_ERROR;
    }
    else
    {
        // 配置串口参数
        rt_device_control(uart3_dev, RT_DEVICE_CTRL_CONFIG, (void *)&uart3_config);
        // 设置接收指示函数
        rt_device_set_rx_indicate(uart3_dev, uart3_function);
    }

    rt_kprintf("UART3 initialized successfully!\n");
    return RT_EOK;
}
#ifdef USING_SERIAL_UART3
    INIT_DEVICE_EXPORT(init_uart3);
#elif defined(USING_BLUETOOTH_UART3)
    INIT_DEVICE_EXPORT(init_uart3);
#endif

