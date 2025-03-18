/*
 * @Description:
            Example of microphone speaker
        Re output the microphone input data from the speaker
 * @Author: LILYGO_L
 * @Date: 2023-12-21 11:30:50
 * @LastEditTime: 2025-03-17 17:58:21
 * @License: GPL 3.0
 */
#include "Arduino_DriveBus_Library.h"
#include "pin_config.h"
#include "codec2.h"

// template <typename T, typename... Args>
// std::unique_ptr<T> _make_unique(Args... arg)
// {
//     return std::unique_ptr{new T(arg...)};
// }

// 44.1 KHz
#define IIS_SAMPLE_RATE 8000 // 采样速率
#define IIS_DATA_BIT 16      // 数据位数

std::vector<short> IIS_Transmission_Data_Stream;

#if defined T3_S3_MVSRBoard_V1_0
std::shared_ptr<Arduino_IIS_DriveBus> IIS_Bus_0 =
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

/**
 * @brief 输入双声道输出单声道
 * @return
 * @Date 2024-10-23 11:27:04
 */
bool IIS_Mono_Conversion(const int16_t *input_data, std::vector<int16_t> *output_data,
                         size_t input_data_size, bool extract_channel)
{
    if (input_data == nullptr || input_data_size < 1)
    {
        log_w("The input data is incorrect.");
        return false;
    }

    if ((input_data_size % 2) != 0)
    {
        log_w("The input data is not an even number and will result in the loss of one data point.");
        input_data_size--;
    }

    // 清空
    output_data->clear();

    if (extract_channel == 0) // 提取左声道
    {
        // 遍历输入 PCM 数据，提取左声道样本
        for (size_t i = 0; i < input_data_size; i += 2)
        {
            output_data->push_back(input_data[i]);
        }
    }
    else // 提取右声道
    {
        // 遍历输入 PCM 数据，提取右声道样本
        for (size_t i = 0; i < input_data_size; i += 2)
        {
            output_data->push_back(input_data[i + 1]);
        }
    }

    return true;
}

/**
 * @brief 输入单声道输出双声道
 * @return
 * @Date 2024-10-23 11:27:04
 */
bool IIS_Dual_Conversion(const int16_t *input_data, std::vector<int16_t> *output_data,
                         size_t input_data_size)
{
    if (input_data == nullptr || input_data_size == 0)
    {
        log_w("The input data is incorrect.");

        return false;
    }

    // 清空
    output_data->clear();

    // 遍历输入样本
    for (size_t i = 0; i < input_data_size; i++)
    {
        // 构建双声道
        output_data->push_back(input_data[i]);
        output_data->push_back(input_data[i]);
    }

    return true;
}

/**
 * @brief 输入双声道处理左右声道数据
 * @return
 * @Date 2024-10-23 11:27:04
 */
bool IIS_Channel_Operation(int16_t *input_data, size_t input_data_size, size_t operation_mode)
{
    if (input_data == nullptr || input_data_size < 1)
    {
        log_w("The input data is incorrect.");

        return false;
    }

    if ((input_data_size % 2) != 0)
    {
        log_w("The input data is not an even number and will result in the loss of one data point.");

        input_data_size--;
    }

    switch (operation_mode)
    {
    case 1: // 将右声道的值复制到左声道
        for (size_t i = 0; i < input_data_size; i++)
        {
            input_data[2 * i] = input_data[2 * i + 1];
        }
        break;
    case 2: // 将左声道的值复制到右声道
        for (size_t i = 0; i < input_data_size; i++)
        {
            input_data[2 * i + 1] = input_data[2 * i];
        }
        break;
    case 3: // 清空左声道值
        for (size_t i = 0; i < input_data_size; i++)
        {
            input_data[2 * i] = 0;
        }
        break;
    case 4: // 清空右声道值
        for (size_t i = 0; i < input_data_size; i++)
        {
            input_data[2 * i + 1] = 0;
        }
        break;

    default:
        break;
    }

    return true;
}

void Codec2_Task(void *parameter)
{
    struct CODEC2 *codec2_state;
    // 麦克风采样次数 1次有8个uint8_t的数据 5次就是40个uint8_t数据
    const uint8_t msm_sample_frequency = 5;

    // construct and configure codec2
    codec2_state = codec2_create(CODEC2_MODE_3200);
    if (codec2_state == NULL)
    {
        Serial.println("Failed to create Codec2");
        return;
    }

    // 设置LPC后置滤波器 提高解码后的语音清晰度
    codec2_set_lpc_post_filter(codec2_state, 1, 0, 0.8, 0.2);

    // 获取Codec2处理每帧音频数据所需要的采样数
    const auto codec2_sample_size = codec2_samples_per_frame(codec2_state);
    // 经过Codec2压缩后的数据量
    const auto codec2_compress_size = codec2_bytes_per_frame(codec2_state);

    // const auto c2_sample_buf_size = sizeof(short) * c2_samples_per_frame_ * sampling_multiplier;
    // auto c2_sample_buf = _make_unique<short[]>(c2_sample_buf_size);

    // auto codec2_compress_buf = _make_unique<unsigned char[]>(codec2_compress_size);

    // 麦克风（单声道）需要采集数据的数量
    const auto msm_sample_buf_size = sizeof(short) * codec2_sample_size;
    auto msm_sample_buf = (short *)malloc(msm_sample_buf_size);

    auto codec2_buf = (short *)malloc(msm_sample_buf_size);

    // 扬声器（双声道）需要播放的数据量
    // const auto max_sample_buf_size = sizeof(short) * codec2_sample_size * 2;

    auto codec2_compress_buf = (unsigned char *)malloc(sizeof(unsigned char) * codec2_compress_size);

    Serial.printf("Codec2 initialization successful\n   codec2_sample_size: %d\n    codec2_compress_size: %d\n",
                  codec2_sample_size, codec2_compress_size);

    // run loopback record-encode-decode-playback loop
    Serial.println("Audio task started");

    while (1)
    {
        vTaskDelay(1);

        for (int i = 0; i < msm_sample_frequency; i++)
        {
            if (IIS->IIS_Read_Data(msm_sample_buf, msm_sample_buf_size) == true)
            {
                std::vector<short> output_buf;

                // for (int i = 0; i < 60; i++)
                // {
                //     Serial.printf("debug1: %d\n", c2_samples_[i]);
                // }
                // Serial.println();

                // uint32_t startTimeEncode = millis();
                codec2_encode(codec2_state, codec2_compress_buf, msm_sample_buf);
                // Serial.println("Done encoding, took ms: " + String(millis() - startTimeEncode));

                // vTaskDelay(1);

                // uint32_t startTimeDecode = millis();
                codec2_decode(codec2_state, codec2_buf, codec2_compress_buf);
                // Serial.println("Done decoding, took ms: " + String(millis() - startTimeDecode));

                IIS_Dual_Conversion(codec2_buf, &output_buf, codec2_sample_size);

                // if (MAX98357A->IIS_Write_Data(output_buf.data(), max_sample_buf_size) == true)
                // {
                //     // Serial.printf("MAX98357A played successfully\n");

                //     // for (int i = 0; i < 25; i++)
                //     // {
                //     //     Serial.printf("debug: %d\n", (int16_t)(temp[i + 2] | temp[i + 3] << 8));
                //     // }

                const auto current_buf_size = IIS_Transmission_Data_Stream.size();

                // Serial.printf("size0: %d\n", output_buf_size);

                // 调整容量
                IIS_Transmission_Data_Stream.resize(current_buf_size + output_buf.size());

                // 存储数据
                // memcpy拷贝的是字节数据
                memcpy(IIS_Transmission_Data_Stream.data() + current_buf_size, output_buf.data(),
                       sizeof(short) * output_buf.size());
            }
        }
        // Serial.printf("size1: %d\n", IIS_Transmission_Data_Stream.size());
    }
}

void MAX_Play_Task(void *parameter)
{
    const short max_play_size = 320;

    while (1)
    {
        vTaskDelay(1);

        if (IIS_Transmission_Data_Stream.size() >= max_play_size) // 流读取判断
        {
            short iis_data_buf[max_play_size];

            // 存储数据
            memcpy(iis_data_buf, IIS_Transmission_Data_Stream.data(), sizeof(short) * max_play_size);

            // 删除已经存储的数据
            IIS_Transmission_Data_Stream.erase(IIS_Transmission_Data_Stream.begin(),
                                               IIS_Transmission_Data_Stream.begin() + max_play_size);

            // Serial.printf("size2: %d\n", IIS_Transmission_Data_Stream.size());

            if (MAX98357A->IIS_Write_Data(iis_data_buf, sizeof(short) * max_play_size) == true)
            {
                // Serial.printf("MAX98357A played successfully\n");

                // for (int i = 0; i < 25; i++)
                // {
                //     Serial.printf("debug: %d\n", (int16_t)(IIS_Write_Buf[i + 2] | IIS_Write_Buf[i + 3] << 8));
                // }
                // vTaskDelay(5);
            }
            else
            {
            }
        }
    }
}

void setup()
{
    Serial.begin(115200);

    pinMode(MAX98357A_SD_MODE, OUTPUT);
    digitalWrite(MAX98357A_SD_MODE, HIGH);

#if defined T3_S3_MVSRBoard_V1_0
    pinMode(MSM261_EN, OUTPUT);
    digitalWrite(MSM261_EN, HIGH);

    // 只有单个麦克风 所以只采集右声道数据
    while (IIS->begin(i2s_mode_t::I2S_MODE_MASTER, ad_iis_data_mode_t::AD_IIS_DATA_IN, i2s_channel_fmt_t::I2S_CHANNEL_FMT_ONLY_RIGHT,
                      IIS_DATA_BIT, IIS_SAMPLE_RATE) == false)
    {
        Serial.println("MSM261 initialization fail");
        delay(2000);
    }
    Serial.println("MSM261 initialization successfully");

#elif defined T3_S3_MVSRBoard_V1_1
    pinMode(MP34DT05TR_EN, OUTPUT);
    digitalWrite(MP34DT05TR_EN, LOW);

    // 只有单个麦克风 所以只采集右声道数据
    while (IIS->begin(i2s_mode_t::I2S_MODE_PDM, ad_iis_data_mode_t::AD_IIS_DATA_IN, i2s_channel_fmt_t::I2S_CHANNEL_FMT_ONLY_RIGHT,
                      IIS_DATA_BIT, IIS_SAMPLE_RATE) == false)
    {
        Serial.println("MP34DT05TR initialization fail");
        delay(2000);
    }
    Serial.println("MP34DT05TR initialization successfully");
#else
#error "Unknown macro definition. Please select the correct macro definition."
#endif

    // 扬声器左和右一半所以输出双声道
    while (MAX98357A->begin(i2s_mode_t::I2S_MODE_MASTER, ad_iis_data_mode_t::AD_IIS_DATA_OUT, i2s_channel_fmt_t::I2S_CHANNEL_FMT_RIGHT_LEFT,
                            IIS_DATA_BIT, IIS_SAMPLE_RATE) == false)
    {
        Serial.println("MAX98357A initialization fail");
        delay(2000);
    }
    Serial.println("MAX98357A initialization successfully");

    // IIS->IIS_Device_Switch(Arduino_IIS::Device_Switch::Channel_OFF);
    // MAX98357A->IIS_Device_Switch(Arduino_IIS::Device_Switch::Channel_OFF);

    xTaskCreate(&Codec2_Task, "Codec2_Task", 32000, NULL, 5, NULL);

    xTaskCreate(&MAX_Play_Task, "MAX_Play_Task", 30000, NULL, 5, NULL);
}

void loop()
{
    delay(100); // xTask time
}