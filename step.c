/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-07-06     Baijiankang       the first version
 */

#include <drv_mpu6xxx.h>
#include "user.h"

rt_int16_t g_step_count = 0;
rt_uint8_t g_fall_flag;
rt_int16_t threshold = 50; // 250
rt_int16_t fall_threshold = 500; 

#define MPU6050_I2C_BUS_NAME          "i2c1"  /* 传感器连接的I2C总线设备名称 */
struct mpu6xxx_3axes accel, gyro;  // 定义存储加速度和陀螺仪数据的结构体变量

/**
 * @brief 步数计算算法 - 峰值检测
 * 
 * @param x 
 * @param y 
 * @param z 
 * @return rt_int16_t 
 */
rt_int16_t StepCounter(rt_int16_t x, rt_int16_t y, rt_int16_t z)
{
  static rt_int16_t prevZ = 0;
  static rt_int16_t stepCount = 0;

  rt_int16_t mag = x * x + y * y + z * z;
  rt_int16_t deltaZ = z - prevZ;
  
  /*步数监测*/
  if (mag > threshold && deltaZ > threshold) {
    stepCount++;
  }
   
  /*判断是否发生了跌倒*/
  if(mag > fall_threshold && deltaZ > fall_threshold)
    g_fall_flag = 1;

  prevZ = z;

  return stepCount;
}

/**
 * @brief 步数计算算法 - 峰值检测
 * 
 * @param x 
 * @param y 
 * @param z 
 * @return  
 */
void Fall_detect(rt_int16_t x, rt_int16_t y, rt_int16_t z)
{
  static rt_int16_t prevZ = 0;

  rt_int16_t mag = x * x + y * y + z * z;
  rt_int16_t deltaZ = z - prevZ;
   
  /*判断是否发生了跌倒*/
  if(mag > fall_threshold && deltaZ > fall_threshold)
    g_fall_flag = 1;

  prevZ = z;
}

/**
 * @brief 步数计算线程入口
 * 
 * @param p 
 */
void step_thread_entry(void *p)
{
    struct mpu6xxx_device *dev;

    // 初始化mpu6xxx设备，MPU6050_I2C_BUS_NAME是I2C总线名称，RT_NULL表示自动探测I2C设备
    dev = mpu6xxx_init(MPU6050_I2C_BUS_NAME, RT_NULL);

    if (dev == RT_NULL)
        rt_kprintf("mpu6xxx_init_failed\n");
    else
        rt_kprintf("mpu6xxx_init_succeed\n");
    while(1)
    {
        mpu6xxx_get_accel(dev, &accel);

        mpu6xxx_get_gyro(dev, &gyro);

        g_step_count = StepCounter(accel.x, accel.y, accel.z);

        Fall_detect(accel.x, accel.y, accel.z);
        
        rt_thread_mdelay(300);
    }
}

/**
 * @brief 初始化步数计算线程
 * 
 * @return rt_int16_t 
 */
rt_int16_t init_step_thread(void)
{
    rt_thread_t step_thread;
    // 创建线程，并启动它
    //-----------------线程------------------------------------------
    step_thread = rt_thread_create("step_thread", step_thread_entry, RT_NULL, 2048, 14, 5);
    if(step_thread != RT_NULL)
        rt_thread_startup(step_thread); // 如果线程创建成功，则启动它
    else
        rt_kprintf("step_thread_failed!\n"); // 如果线程创建失败，则打印错误信息
    return RT_EOK;
}
//INIT_DEVICE_EXPORT(step_thread);






