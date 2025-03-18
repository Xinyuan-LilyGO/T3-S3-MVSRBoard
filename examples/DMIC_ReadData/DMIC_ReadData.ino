/*
 * @Description:
            DMIC Test
        Print microphone loudness values on computer serial port
 * @Author: LILYGO_L
 * @Date: 2023-08-17 16:24:06
 * @LastEditTime: 2025-03-17 17:31:11
 * @License: GPL 3.0
 */
#include "Arduino_DriveBus_Library.h"
#include "pin_config.h"

#define IIS_SAMPLE_RATE 44100 // 采样速率
#define IIS_DATA_BIT 16       // 数据位数

#if defined T3_S3_MVSRBoard_V1_0
std::shared_ptr<Arduino_IIS_DriveBus> IIS_Bus =
    std::make_shared<Arduino_HWIIS>(I2S_NUM_0, MSM261_BCLK, MSM261_WS, MSM261_DATA);
#elif defined T3_S3_MVSRBoard_V1_1
std::shared_ptr<Arduino_IIS_DriveBus> IIS_Bus =
    std::make_shared<Arduino_HWIIS>(I2S_NUM_0, -1, MP34DT05TR_LRCLK, MP34DT05TR_DATA);
#else
#error "Unknown macro definition. Please select the correct macro definition."
#endif

std::unique_ptr<Arduino_IIS> IIS(new Arduino_MEMS(IIS_Bus));

char IIS_Read_Buff[1000];

void setup()
{
    Serial.begin(115200);

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
}

void loop()
{
    if (IIS->IIS_Read_Data(IIS_Read_Buff, 512 / 4) == true)
    {
        // 输出右声道数据
        Serial.printf("Right: %d\n", (int16_t)(IIS_Read_Buff[2] | IIS_Read_Buff[3] << 8));

        // 输出左声道数据
        Serial.printf("Left: %d\n", (int16_t)(IIS_Read_Buff[0] | IIS_Read_Buff[1] << 8));

        // Serial.print((int16_t)(IIS_Read_Buff[2] | IIS_Read_Buff[3] << 8)); // Arduino
        // Serial.print(",");
        // Serial.println((int16_t)(IIS_Read_Buff[0] | IIS_Read_Buff[1] << 8));

        // for (int i = 0; i < 512 / 4; i++)
        // {
        //     Serial.printf("debug1[%d]: %d\n", i, IIS_Read_Buff[i]);
        // }
        // Serial.println();
    }
    else
    {
        Serial.printf("Failed to read IIS data");
    }

    delay(50);
}
