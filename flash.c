
 /*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-06-29     Baijiankang       the first version
 */

#include <rtthread.h>
#include <fal.h>
#define DBG_TAG "flash"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
#include "user.h"
#include "uart.h"

#define RECORD_NUM 3 // max 1024
int16_t data_count = 0;
size_t used_size = 0; // 假设 used_size 是全局变量，记录已使用的空间

typedef struct my_flash_save_data
{
    char name[10];
    int8_t HR_data;
    int16_t SP02_data;
    char result[2]; // 分析结果 good, bad, normal
} my_flash_save_data_t;

/**
 * @brief 读取结构体数组
 * 
 * @param partition_name 分区名
 * @param addr  偏移地址
 * @param data  结构体数组
 * @param count 结构体数组长度
 */
void read_struct_data(const char *partition_name, uint32_t addr, my_flash_save_data_t *data, uint32_t count)
{
    const struct fal_partition *partition;

    // 查找分区
    partition = fal_partition_find(partition_name);
    if (partition == NULL) {
        rt_kprintf("Partition not found: %s\n", partition_name);
        return;
    }

    // 读取数据
    if (fal_partition_read(partition, addr, (uint8_t *)data, count * sizeof(my_flash_save_data_t)) != count * sizeof(my_flash_save_data_t)) {
        rt_kprintf("Failed to read data from partition: %s\n", partition_name);
        return;
    }
}

/**
 * @brief 写入结构体数组到flash
 * 
 * @param partition_name 分区名
 * @param addr 偏移地址
 * @param data 结构体数组
 * @param count 结构体数组长度
 * @return void 
 */
void write_struct_data(const char *partition_name, uint32_t addr, const my_flash_save_data_t *data, uint32_t count)
{
    const struct fal_partition *partition;

    // 查找分区
    partition = fal_partition_find(partition_name);
    if (partition == NULL) {
        rt_kprintf("Partition not found: %s\n", partition_name);
        return;
    }

    // 擦除分区
    fal_partition_erase(partition, addr, count * sizeof(my_flash_save_data_t));

    // 写入数据
    if (fal_partition_write(partition, addr, (const uint8_t *)data, count * sizeof(my_flash_save_data_t)) != count * sizeof(my_flash_save_data_t)) {
        rt_kprintf("Failed to write data to partition: %s\n", partition_name);
        return;
    }

    data_count += count;
    used_size += count * sizeof(my_flash_save_data_t);

    rt_kprintf("Data written successfully.\n");
}

/**
 * @brief 记录数据到flash
 * 
 * @param data 
 * @return void 
 */
void data_record(my_flash_save_data_t *data)
{
    rt_kprintf("Writing structures to partition '%s' at address %u\n", "part4", 0);

    // 写入结构体数组
    write_struct_data("part4", 0, data, 1);
}

/**
 * @brief uart展示flash数据
 * 
 * @sample data_display_by_uart(uart2_dev, 3);
 */
void flash_data_display_by_uart(rt_device_t uartx_dev, int16_t number)
{
    my_flash_save_data_t read_data_array[number];
    char str[100];
    read_struct_data("part4", 0, read_data_array, number);

    for (int i = 0; i < number; i++) {
        sprintf(str, read_data_array[i].name, read_data_array[i].HR_data,
                   read_data_array[i].SP02_data, read_data_array[i].result);
        uart_putstring(uartx_dev, str);
    }
}

/*待调试*/
void get_partition_remaining_size(const char *partition_name)
{
    const struct fal_partition *partition = fal_partition_find(partition_name);
    if (partition != NULL) {
        size_t total_size = partition->len;

        // 计算剩余空间大小
        size_t remaining_size = total_size - used_size;
        rt_kprintf("Remaining size of partition '%s': %u bytes\n", partition_name, remaining_size);
    } else {
        rt_kprintf("Partition '%s' not found!\n", partition_name);
    }
}

/**
 * @brief flash线程入口函数
 * 
 */
void flash_thread_entry(void *p)
{
    int count = 1;

    // 初始化FAL
    fal_init();

    while (count++) {
        rt_thread_mdelay(1000);
    }
}

/**
 * @brief 初始化flash线程
 * 
 * @return int 
 */
rt_int16_t init_flash_thread(void)
{
    rt_thread_t flash_thread;
    // 创建线程，并启动它
    //-----------------线程------------------------------------------
    flash_thread = rt_thread_create("flash", flash_thread_entry, RT_NULL, 65536, 12, 5);
    if(flash_thread != RT_NULL)
        rt_thread_startup(flash_thread); // 如果线程创建成功，则启动它
    else
        {
            rt_kprintf("flash_thread_failed!\n"); // 如果线程创建失败，则打印错误信息
            return RT_ERROR;
        }

    return RT_EOK;
}
//INIT_DEVICE_EXPORT(flash_thread);
