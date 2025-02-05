/*
 * @Description: Arduino_IIS.cpp
 * @version: V1.0.0
 * @Author: Xk_w
 * @Date: 2023-11-16 16:58:05
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2024-11-05 14:18:21
 * @License: GPL 3.0
 */
#include "Arduino_IIS.h"

Arduino_IIS::Arduino_IIS(std::shared_ptr<Arduino_IIS_DriveBus> bus)
    : _bus(bus)
{
}

bool Arduino_IIS::begin(int8_t device_state, int32_t sample_rate, int32_t bits_per_sample,
                        int8_t channel_mode)
{
    if (_bus->begin(device_state, sample_rate, bits_per_sample, channel_mode) == false)
    {
        log_e("->_bus->begin(device_state, sample_rate, bits_per_sample) fail");
        return false;
    }

    return true;
}

bool Arduino_IIS::IIS_Read_Data(void *data, size_t bytes)
{
    log_e("No 'IIS_Read_Data' fictional function has been created.");
    return -1;
}

bool Arduino_IIS::IIS_Write_Data(const void *data, size_t bytes)
{
    log_e("No 'IIS_Write_Data' fictional function has been created.");
    return -1;
}

bool Arduino_IIS::end()
{
    if (_bus->end() == false)
    {
        log_e("->_bus->end() fail");
        return false;
    }

    return true;
}
