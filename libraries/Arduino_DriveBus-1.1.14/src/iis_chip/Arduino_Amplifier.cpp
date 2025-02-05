/*
 * @Description: Arduino_Amplifier.cpp
 * @version: V1.0.0
 * @Author: Xk_w
 * @Date: 2023-12-21 14:04:34
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2024-11-05 14:15:52
 * @License: GPL 3.0
 */

#include "Arduino_Amplifier.h"

Arduino_Amplifier::Arduino_Amplifier(std::shared_ptr<Arduino_IIS_DriveBus> bus)
    : Arduino_IIS(bus)
{
}

bool Arduino_Amplifier::begin(int8_t device_state, int32_t sample_rate,
                              int32_t bits_per_sample, int8_t channel_mode)
{
    return Arduino_IIS::begin(device_state, sample_rate, bits_per_sample, channel_mode);
}

bool Arduino_Amplifier::IIS_Write_Data(const void *data, size_t bytes)
{
    return _bus->IIS_Write(data, bytes);
}

bool Arduino_Amplifier::end()
{
    return Arduino_IIS::end();
}