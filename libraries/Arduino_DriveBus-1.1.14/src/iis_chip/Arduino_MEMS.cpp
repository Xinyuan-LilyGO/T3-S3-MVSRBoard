/*
 * @Description: Arduino_MEMS.cpp
 * @version: V1.0.0
 * @Author: Xk_w
 * @Date: 2023-12-21 14:04:34
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2024-11-05 14:16:05
 * @License: GPL 3.0
 */

#include "Arduino_MEMS.h"

Arduino_MEMS::Arduino_MEMS(std::shared_ptr<Arduino_IIS_DriveBus> bus)
    : Arduino_IIS(bus)
{
}

bool Arduino_MEMS::begin(int8_t device_state, int32_t sample_rate, int32_t bits_per_sample,
                         int8_t channel_mode)
{
    return Arduino_IIS::begin(device_state, sample_rate, bits_per_sample, channel_mode);
}

bool Arduino_MEMS::IIS_Read_Data(void *data, size_t bytes)
{
    return _bus->IIS_Read(data, bytes);
}

bool Arduino_MEMS::end()
{
    return Arduino_IIS::end();
}