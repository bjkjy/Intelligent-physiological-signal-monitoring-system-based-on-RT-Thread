/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-06-28     Baijiankang       the first version
 */
#ifndef DRIVERS_INCLUDE_DRV_ALGORITHM_H_
#define DRIVERS_INCLUDE_DRV_ALGORITHM_H_


#define FFT_N           1024    //定义傅里叶变换的点数
#define START_INDEX     0       //低频过滤阈值

struct compx        //定义一个复数结构
{
    float real;
    float imag;
};

typedef struct      //定义一个直流滤波器结构体
{
    float w;
    int init;
    float a;
}DC_FilterData;     //用于存储直流滤波器的参数


typedef struct      //定义一个带宽滤波器结构体
{
    float v0;
    float v1;
}BW_FilterData;     //用于存储带宽滤波器的参数



double my_floor(double x);

double my_fmod(double x, double y);

double XSin( double x );

double XCos( double x );

int qsqrt(int a);


struct compx EE(struct compx a,struct compx b);

void FFT(struct compx *xin);

int find_max_num_index(struct compx *data,int count);
int dc_filter(int input,DC_FilterData * df);
int bw_filter(int input,BW_FilterData * bw);
void ButterworthBandPassFilter(float new_data, float *prev_output1, float *prev_output2, float *prev_input1, float *prev_input2, float center_freq, float bandwidth, float sample_rate, float *b, float *a);

#endif /* DRIVERS_INCLUDE_DRV_ALGORITHM_H_ */
