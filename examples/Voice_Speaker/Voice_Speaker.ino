/*
 * @Description:
            Example of microphone speaker
        Re output the microphone input data from the speaker
 * @Author: LILYGO_L
 * @Date: 2023-12-21 11:30:50
 * @LastEditTime: 2025-03-17 18:03:29
 * @License: GPL 3.0
 */
#include "Arduino_DriveBus_Library.h"
#include "pin_config.h"

// 44.1 KHz
#define IIS_SAMPLE_RATE 44100 // 采样速率
#define IIS_DATA_BIT 16       // 数据位数

char IIS_Read_Buff[100];

#if defined T3_S3_MVSRBoard_V1_0
std::shared_ptr<Arduino_IIS_DriveBus> IIS_Bus =
    std::make_shared<Arduino_HWIIS>(I2S_NUM_0, MSM261_BCLK, MSM261_WS, MSM261_DATA);
#elif defined T3_S3_MVSRBoard_V1_1
std::shared_ptr<Arduino_IIS_DriveBus> IIS_Bus_0 =
    std::make_shared<Arduino_HWIIS>(I2S_NUM_0, -1, MP34DT05TR_LRCLK, MP34DT05TR_DATA);
#else
#error "Unknown macro definition. Please select the correct macro definition."
#endif

std::unique_ptr<Arduino_IIS> IIS(new Arduino_MEMS(IIS_Bus_0));

std::shared_ptr<Arduino_IIS_DriveBus> IIS_Bus_1 =
    std::make_shared<Arduino_HWIIS>(I2S_NUM_1, MAX98357A_BCLK, MAX98357A_LRCLK,
                                    MAX98357A_DATA);

std::unique_ptr<Arduino_IIS> MAX98357A(new Arduino_Amplifier(IIS_Bus_1));

bool IIS_Mono_Processing(char *data, int32_t length, bool channel)
{
    if (length < 0 || data == NULL)
    {
        return false;
    }

    // 右声道复制到左声道
    if (channel == 0)
    {
        for (uint32_t i = 0; i < length; i += 4)
        {
            memmove(&data[i], &data[i + 2], 2);
        }
    }

    // 左声道复制到右声道
    if (channel == 1)
    {
        for (uint32_t i = 0; i < length; i += 4)
        {
            memmove(&data[i + 2], &data[i], 2);
        }
    }

    return true;
}

void setup()
{
    Serial.begin(115200);

    pinMode(MAX98357A_SD_MODE, OUTPUT);
    digitalWrite(MAX98357A_SD_MODE, HIGH);

#if defined T3_S3_MVSRBoard_V1_0
    pinMode(MSM261_EN, OUTPUT);
    digitalWrite(MSM261_EN, HIGH);

    while (IIS->begin(i2s_mode_t::I2S_MODE_MASTER, ad_iis_data_mode_t::AD_IIS_DATA_IN, i2s_channel_fmt_t::I2S_CHANNEL_FMT_RIGHT_LEFT,
                      IIS_DATA_BIT, IIS_SAMPLE_RATE) == false)
    {
        Serial.println("MSM261 initialization fail");
        delay(2000);
    }
    Serial.println("MSM261 initialization successfully");

#elif defined T3_S3_MVSRBoard_V1_1
    pinMode(MP34DT05TR_EN, OUTPUT);
    digitalWrite(MP34DT05TR_EN, LOW);

    while (IIS->begin(i2s_mode_t::I2S_MODE_PDM, ad_iis_data_mode_t::AD_IIS_DATA_IN, i2s_channel_fmt_t::I2S_CHANNEL_FMT_RIGHT_LEFT,
                      IIS_DATA_BIT, IIS_SAMPLE_RATE) == false)
    {
        Serial.println("MP34DT05TR initialization fail");
        delay(2000);
    }
    Serial.println("MP34DT05TR initialization successfully");
#else
#error "Unknown macro definition. Please select the correct macro definition."
#endif

    while (MAX98357A->begin(i2s_mode_t::I2S_MODE_MASTER, ad_iis_data_mode_t::AD_IIS_DATA_OUT, i2s_channel_fmt_t::I2S_CHANNEL_FMT_RIGHT_LEFT,
                            IIS_DATA_BIT, IIS_SAMPLE_RATE) == false)
    {
        Serial.println("MAX98357A initialization fail");
        delay(2000);
    }
    Serial.println("MAX98357A initialization successfully");

    // IIS->IIS_Device_Switch(Arduino_IIS::Device_Switch::Channel_OFF);
    // MAX98357A->IIS_Device_Switch(Arduino_IIS::Device_Switch::Channel_OFF);
}

void loop()
{
    if (IIS->IIS_Read_Data(IIS_Read_Buff, 12) == true)
    {

        // 单声道处理
        IIS_Mono_Processing(IIS_Read_Buff, 12, 0);

        if (MAX98357A->IIS_Write_Data(IIS_Read_Buff, 12) == true)
        {
            // Serial.printf("MAX98357A played successfully\n");
        }

        // 输出左声道数据
        // Serial.printf("Left: %d\n", (int16_t)((int16_t)IIS_Read_Buff[0] | (int16_t)IIS_Read_Buff[1] << 8));

        // 输出右声道数据
        // Serial.printf("Right: %d\n", (int16_t)((int16_t)IIS_Read_Buff[2] | (int16_t)IIS_Read_Buff[3] << 8));

        // Arduino
        Serial.println((int16_t)((int16_t)IIS_Read_Buff[0] | (int16_t)IIS_Read_Buff[1] << 8));
        Serial.print(",");
        Serial.print((int16_t)((int16_t)IIS_Read_Buff[2] | (int16_t)IIS_Read_Buff[3] << 8));
    }
    else
    {
        Serial.printf("Failed to read IIS data");
    }

    // delay(1);
}