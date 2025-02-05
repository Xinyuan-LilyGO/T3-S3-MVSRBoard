/*
 * @Description(CN): MEMS相关麦克风均兼容
 *
 * @Description(EN): All MEMS-related microphones are compatible.
 *
 * @version: V1.0.0
 * @Author: Xk_w
 * @Date: 2023-12-21 14:23:07
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2024-11-04 17:05:08
 * @License: GPL 3.0
 */
#pragma once

#include "../Arduino_IIS.h"

class Arduino_MEMS : public Arduino_IIS
{
public:
    Arduino_MEMS(std::shared_ptr<Arduino_IIS_DriveBus> bus);

    bool begin(int8_t device_state = DRIVEBUS_DEFAULT_VALUE,
               int32_t sample_rate = 44100UL, int32_t bits_per_sample = 16,
               int8_t channel_mode = 0) override;

    bool IIS_Read_Data(void *data, size_t bytes) override;

    bool end() override;
};