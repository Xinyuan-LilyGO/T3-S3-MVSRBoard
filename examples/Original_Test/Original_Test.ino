/*
 * @Description: 出厂测试
 * @Author: LILYGO_L
 * @Date: 2024-10-28 18:03:22
 * @LastEditTime: 2025-03-28 15:38:15
 * @License: GPL 3.0
 */

#include <SPI.h>
#include <Wire.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "pin_config.h"
#include "Display_Fonts.h"
#include <WiFi.h>
#include "Arduino_DriveBus_Library.h"
#include "Audio.h"
#include "RadioLib.h"
#include "codec2.h"

#ifdef T3_S3_SX1262
#define SOFTWARE_NAME "Original_Test_SX1262"
#endif
#ifdef T3_S3_SX1276
#define SOFTWARE_NAME "Original_Test_SX1276"
#endif
#ifdef T3_S3_SX1278
#define SOFTWARE_NAME "Original_Test_SX1278"
#endif
#ifdef T3_S3_SX1280
#define SOFTWARE_NAME "Original_Test_SX1280"
#endif
#ifdef T3_S3_SX1280PA
#define SOFTWARE_NAME "Original_Test_SX1280PA"
#endif

#define SOFTWARE_LASTEDITTIME "202503281536"

#if defined T3_S3_MVSRBoard_V1_0
#define BOARD_VERSION "V1.0"
#elif defined T3_S3_MVSRBoard_V1_1
#define BOARD_VERSION "V1.1"
#else
#error "Unknown macro definition. Please select the correct macro definition."
#endif

#define WIFI_SSID "xinyuandianzi"
#define WIFI_PASSWORD "AA15994823428"
// #define WIFI_SSID "LilyGo-AABB"
// #define WIFI_PASSWORD "xinyuandianzi"

#define WIFI_CONNECT_WAIT_MAX 5000

#define IIS_SAMPLE_RATE 8000 // 采样速率
#define IIS_DATA_BIT 16      // 数据位数

#define LORA_TRANSMISSION_HEAD_SIZE 12

#define LORA_TRANSMISSION_DATA_SIZE 240

#define SD_FILE_NAME "/music.mp3"

struct Button_Triggered_Operator
{
    using gesture = enum {
        NOT_ACTIVE,   // 未激活
        SINGLE_CLICK, // 单击
        DOUBLE_CLICK, // 双击
        LONG_PRESS,   // 长按
    };
    const uint32_t button_number = BOOT_KEY;

    size_t cycletime_1 = 0;
    size_t cycletime_2 = 0;

    uint8_t current_state = gesture::NOT_ACTIVE;
    bool trigger_start_flag = false;
    bool trigger_flag = false;
    bool timing_flag = false;
    int8_t paragraph_triggered_level = -1;
    uint8_t high_triggered_count = 0;
    uint8_t low_triggered_count = 0;
    uint8_t paragraph_triggered_count = 0;

    volatile bool Interrupt_Flag = false;
};

const char *fileDownloadUrl = "https://freetyst.nf.migu.cn/public/product9th/product45/2022/05/0716/2018%E5%B9%B409%E6%9C%8812%E6%97%A510%E7%82%B943%E5%88%86%E7%B4%A7%E6%80%A5%E5%86%85%E5%AE%B9%E5%87%86%E5%85%A5%E5%8D%8E%E7%BA%B3179%E9%A6%96/%E6%A0%87%E6%B8%85%E9%AB%98%E6%B8%85/MP3_128_16_Stero/6005751EPFG164228.mp3?channelid=02&msisdn=d43a7dcc-8498-461b-ba22-3205e9b6aa82&Tim=1728484238063&Key=0442fa065dacda7c";

const uint64_t Local_MAC = ESP.getEfuseMac();

std::vector<short> IIS_Read_Data_Stream;
std::vector<short> IIS_Write_Data_Stream;
std::vector<unsigned char> Lora_Send_Data_Stream;
std::vector<unsigned char> Lora_Receive_Data_Stream;

bool Lora_Transmission_Mode = 0;     // 默认为接收模式
float Lora_Receive_RSSI_Value = 0;   // 接收数据的RSSI值
float Lora_Receive_SNR_Value = 0;    // 接收数据的SNR值
uint64_t Lora_Receive_MAC = 0;       // 接收数据的MAC地址值
bool Lora_Info_Refresh_Flag = false; // Lora信息刷新标志

bool Volume_Change_Flag = 0;
uint8_t Music_Start_Playing_Count = 0;
bool Music_Start_Playing_Flag = true;

bool Skip_Current_Test = false;

size_t CycleTime = 0;
size_t CycleTime_2 = 0;

bool Vibration_Start_Flag = false;

bool Wifi_Connection_Flag = true;

bool Radio_Initialization_Flag = false;

bool PCF85063_Initialization_Flag = false;

uint8_t Send_Package[LORA_TRANSMISSION_HEAD_SIZE + LORA_TRANSMISSION_DATA_SIZE] = {
    'M',
    'A',
    'C',
    ':',
    (uint8_t)(Local_MAC >> 56),
    (uint8_t)(Local_MAC >> 48),
    (uint8_t)(Local_MAC >> 40),
    (uint8_t)(Local_MAC >> 32),
    (uint8_t)(Local_MAC >> 24),
    (uint8_t)(Local_MAC >> 16),
    (uint8_t)(Local_MAC >> 8),
    (uint8_t)Local_MAC,
};

uint8_t Receive_Package[LORA_TRANSMISSION_HEAD_SIZE + LORA_TRANSMISSION_DATA_SIZE];

// flag to indicate that a packet was sent or received
volatile bool Radio_Operation_Flag = false;

std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
    std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire1);

void Arduino_IIC_Touch_Interrupt(void);

std::unique_ptr<Arduino_IIC> PCF85063(new Arduino_PCF85063(IIC_Bus, PCF85063_DEVICE_ADDRESS,
                                                           DRIVEBUS_DEFAULT_VALUE, PCF85063_INT, Arduino_IIC_Touch_Interrupt));

#if defined T3_S3_MVSRBoard_V1_0
std::shared_ptr<Arduino_IIS_DriveBus> IIS_Bus_0 =
    std::make_shared<Arduino_HWIIS>(I2S_NUM_0, IIS_BCLK, IIS_WS, IIS_DATA);
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

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, SCREEN_RST);

Button_Triggered_Operator Button_Triggered_OP;

Audio audio(false, 3, I2S_NUM_1);

#ifdef T3_S3_SX1262
SX1262 radio = new Module(LORA_CS, LORA_DIO1, LORA_RST, LORA_BUSY, SPI);
#endif

#ifdef T3_S3_SX1276
SX1276 radio = new Module(LORA_CS, LORA_DIO0, LORA_RST, LORA_DIO1, SPI);
#endif

#ifdef T3_S3_SX1278
SX1278 radio = new Module(LORA_CS, LORA_DIO0, LORA_RST, LORA_DIO1, SPI);
#endif

#if defined(T3_S3_SX1280) || defined(T3_S3_SX1280PA)
SX1280 radio = new Module(LORA_CS, LORA_DIO1, LORA_RST, LORA_BUSY, SPI);
#endif

TaskHandle_t Codec2_Task_Handle = NULL;
TaskHandle_t MAX_Play_Task_Handle = NULL;

SPIClass SPI_2(FSPI);

void Arduino_IIC_Touch_Interrupt(void)
{
    PCF85063->IIC_Interrupt_Flag = true;
}

void Radio_Interrupt(void)
{
    // we sent or received a packet, set the flag
    Radio_Operation_Flag = true;
}

void Vibration_Start(void)
{
    ledcWrite(2, 255);
    delay(200);
    ledcWrite(2, 0);
}

bool Key_Scanning(void)
{
    if (Button_Triggered_OP.trigger_start_flag == false)
    {
        if (digitalRead(Button_Triggered_OP.button_number) == LOW) // 低电平触发
        {
            Button_Triggered_OP.trigger_start_flag = true;

            Serial.println("Press button to trigger start");
            Button_Triggered_OP.high_triggered_count = 0;
            Button_Triggered_OP.low_triggered_count = 0;
            Button_Triggered_OP.paragraph_triggered_count = 0;
            Button_Triggered_OP.paragraph_triggered_level = -1;
            Button_Triggered_OP.trigger_flag = true;
            Button_Triggered_OP.timing_flag = true;
        }
    }
    if (Button_Triggered_OP.timing_flag == true)
    {
        Button_Triggered_OP.cycletime_2 = millis() + 1000; // 计时1000ms关闭
        Button_Triggered_OP.timing_flag = false;
    }
    if (Button_Triggered_OP.trigger_flag == true)
    {
        if (millis() > Button_Triggered_OP.cycletime_1)
        {
            if (digitalRead(Button_Triggered_OP.button_number) == HIGH)
            {
                Button_Triggered_OP.high_triggered_count++;
                if (Button_Triggered_OP.paragraph_triggered_level != HIGH)
                {
                    Button_Triggered_OP.paragraph_triggered_count++;
                    Button_Triggered_OP.paragraph_triggered_level = HIGH;
                }
            }
            else if (digitalRead(Button_Triggered_OP.button_number) == LOW)
            {
                Button_Triggered_OP.low_triggered_count++;
                if (Button_Triggered_OP.paragraph_triggered_level != LOW)
                {
                    Button_Triggered_OP.paragraph_triggered_count++;
                    Button_Triggered_OP.paragraph_triggered_level = LOW;
                }
            }
            Button_Triggered_OP.cycletime_1 = millis() + 50;
        }

        if (Button_Triggered_OP.timing_flag == false)
        {
            if (millis() > Button_Triggered_OP.cycletime_2)
            {
                Serial.print("end\n");
                Serial.printf("high_triggered_count: %d\n", Button_Triggered_OP.high_triggered_count);
                Serial.printf("low_triggered_count: %d\n", Button_Triggered_OP.low_triggered_count);
                Serial.printf("paragraph_triggered_count: %d\n", Button_Triggered_OP.paragraph_triggered_count);

                Button_Triggered_OP.trigger_flag = false;
                Button_Triggered_OP.trigger_start_flag = false;

                if ((Button_Triggered_OP.paragraph_triggered_count == 2)) // 单击
                {
                    Button_Triggered_OP.current_state = Button_Triggered_OP.gesture::SINGLE_CLICK;
                    return true;
                }
                else if ((Button_Triggered_OP.paragraph_triggered_count == 4)) // 双击
                {
                    Button_Triggered_OP.current_state = Button_Triggered_OP.gesture::DOUBLE_CLICK;
                    return true;
                }
                else if ((Button_Triggered_OP.paragraph_triggered_count == 1)) // 长按
                {
                    Button_Triggered_OP.current_state = Button_Triggered_OP.gesture::LONG_PRESS;
                    return true;
                }
            }
        }
    }

    return false;
}

/**
 * @brief 输入单声道输出双声道
 * @return
 * @Date 2024-10-23 11:27:04
 */
bool IIS_Dual_Conversion(const int16_t *input_data, std::vector<int16_t> *output_data,
                         size_t input_data_size, float decibel_multiplier = 1.0)
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
        output_data->push_back((short)((float)input_data[i] * decibel_multiplier));
        output_data->push_back((short)((float)input_data[i] * decibel_multiplier));
    }

    return true;
}

void GFX_Print_Music_Volume_Value()
{
    display.fillRect(5, SCREEN_HEIGHT / 3, 128, 30, BLACK);
    display.setCursor(20, SCREEN_HEIGHT / 2 - 5);
    display.setFont(NULL);
    display.setTextSize(1);
    display.printf("Volume:%d", audio.getVolume());

    display.display();
}

void GFX_Print_SX12xx_Walkie_Talkie_Info_Loop()
{
    display.fillRect(50, 15, 78, 30, BLACK);
    display.fillRect(0, 55, 128, 9, BLACK);

    if (Lora_Transmission_Mode == 0) // 接收模式
    {
        display.setFont(&FreeMonoBold9pt7b);
        display.setTextSize(1);
        display.setCursor(57, 26);
        display.printf("RX");

        display.setFont(&Org_01);
        display.setCursor(57, 35);
        display.printf("RSSI:%.1fdBm", Lora_Receive_RSSI_Value);
        display.setCursor(57, 42);
        display.printf("SNR:%.1fdB", Lora_Receive_SNR_Value);

        display.setCursor(0, 60);
        display.printf("E_MAC:%llu", Lora_Receive_MAC);
    }
    else // 发送模式
    {
        display.setFont(&FreeMonoBold9pt7b);
        display.setTextSize(1);
        display.setCursor(57, 26);
        display.printf("TX");

        display.setFont(&Org_01);
        display.setCursor(57, 35);
        display.printf("RSSI:NULL");
        display.setCursor(57, 42);
        display.printf("SNR:NULL");

        display.setCursor(0, 60);
        display.printf("E_MAC:NULL");
    }

    display.display();
}

void Wifi_STA_Test(void)
{
    String text;
    int wifi_num = 0;

    display.fillScreen(BLACK);
    display.setFont(NULL);
    display.setCursor(0, 0);
    display.setTextSize(1);

    Serial.println("\nScanning wifi");
    display.printf("Scanning wifi\n");
    display.display();
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    WiFi.setTxPower(WIFI_POWER_19_5dBm);
    delay(100);

    wifi_num = WiFi.scanNetworks();
    if (wifi_num == 0)
    {
        text = "\nWiFi scan complete !\nNo wifi discovered.\n";
    }
    else
    {
        text = "\nWiFi scan complete !\n";
        text += wifi_num;
        text += " wifi discovered.\n\n";

        for (int i = 0; i < wifi_num; i++)
        {
            text += (i + 1);
            text += ": ";
            text += WiFi.SSID(i);
            text += " (";
            text += WiFi.RSSI(i);
            text += ")";
            text += (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " \n" : "*\n";
            delay(10);
        }
    }

    Serial.println(text);
    display.println(text);
    display.display();

    delay(3000);
    text.clear();
    display.fillScreen(BLACK);
    display.setCursor(0, 0);

    Serial.print("Connecting to ");
    display.printf("Connecting to\n");

    Serial.print(WIFI_SSID);
    display.printf("%s", WIFI_SSID);
    display.display();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    uint32_t last_tick = millis();

    Wifi_Connection_Flag = true;

    while (WiFi.status() != WL_CONNECTED)
    {
        if (millis() > CycleTime)
        {
            Serial.print(".");
            display.printf(".");
            display.display();

            CycleTime = millis() + 3000;
        }

        if (millis() - last_tick > WIFI_CONNECT_WAIT_MAX)
        {
            Wifi_Connection_Flag = false;
            break;
        }
    }

    if (Wifi_Connection_Flag == true)
    {
        Serial.print("\nThe connection was successful ! \nTakes ");
        display.printf("\nThe connection was successful ! \nTakes ");
        Serial.print(millis() - last_tick);
        display.print(millis() - last_tick);
        Serial.println(" ms\n");
        display.printf(" ms\n");

        display.printf("\nWifi test passed!");
    }
    else
    {
        display.printf("\nWifi test error!\n");
    }
    display.display();
}

void SD_Music_Test(void)
{
    display.fillScreen(BLACK);
    display.setFont(NULL);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Trying to play SD music...");
    display.display();

    SPI_2.setFrequency(16000000);
    SPI_2.begin(SD_SCLK, SD_MISO, SD_MOSI);

    if (SD.begin(SD_CS, SPI_2) == true)
    {
        audio.connecttoSD(SD_FILE_NAME);
        Music_Start_Playing_Flag = true;
        Music_Start_Playing_Count = 0;
    }
    else
    {
        Music_Start_Playing_Flag = false;
        Music_Start_Playing_Count = 31;
    }

    while (audio.getAudioCurrentTime() <= 0)
    {
        audio.loop();

        if (millis() > CycleTime)
        {
            Music_Start_Playing_Count++;
            if (Music_Start_Playing_Count > 10) // 10秒后下载超时
            {
                Music_Start_Playing_Flag = false;
                break;
            }
            display.print(".");
            display.display();

            CycleTime = millis() + 1000;
        }
    }

    display.fillScreen(BLACK);

    display.setCursor(30, 5);
    display.setTextSize(1);
    display.setFont(NULL);
    if (Music_Start_Playing_Flag == true)
    {
        display.printf("Play Info\n");

        GFX_Print_Music_Volume_Value();
    }
    else
    {
        display.printf("Play Failed\n");
    }

    display.display();
}

void Wifi_Music_Test(void)
{
    display.fillScreen(BLACK);
    display.setFont(NULL);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Trying to play Wifi music...");
    display.display();

    audio.connecttohost(fileDownloadUrl);

    Music_Start_Playing_Flag = true;
    Music_Start_Playing_Count = 0;
    while (audio.getAudioCurrentTime() <= 0)
    {
        audio.loop();

        if (millis() > CycleTime)
        {
            Music_Start_Playing_Count++;
            if (Music_Start_Playing_Count > 10) // 10秒后下载超时
            {
                Music_Start_Playing_Flag = false;
                break;
            }
            display.print(".");
            display.display();

            CycleTime = millis() + 1000;
        }
    }

    display.fillScreen(BLACK);

    display.setCursor(30, 5);
    display.setTextSize(1);
    display.setFont(NULL);
    if (Music_Start_Playing_Flag == true)
    {
        display.printf("Play Info\n");

        GFX_Print_Music_Volume_Value();

        display.display();
    }
    else
    {
        SD_Music_Test();
    }
}

void Skip_Test_Loop()
{
    if (Key_Scanning() == true)
    {
        switch (Button_Triggered_OP.current_state)
        {
        case Button_Triggered_OP.gesture::SINGLE_CLICK:
            Serial.println("Key triggered: SINGLE_CLICK");

            // delay(300);
            break;
        case Button_Triggered_OP.gesture::DOUBLE_CLICK:
            Serial.println("Key triggered: DOUBLE_CLICK");

            // delay(1000);
            break;
        case Button_Triggered_OP.gesture::LONG_PRESS:
            Serial.println("Key triggered: LONG_PRESS");

            Skip_Current_Test = true;

            // delay(1000);
            break;

        default:
            break;
        }
    }
}

void GFX_Print_TEST(String s)
{
    display.fillScreen(BLACK);
    display.setFont(&FreeSans9pt7b);
    display.setTextSize(1);

    display.setCursor(SCREEN_WIDTH / 4 + 5, SCREEN_HEIGHT / 4);
    display.printf("TEST");

    display.setFont(NULL);
    display.setTextSize(1);
    display.setCursor(5, SCREEN_HEIGHT / 4 + 10);
    display.print(s);

    display.setCursor(SCREEN_WIDTH / 2 - 5, SCREEN_HEIGHT / 2 + 20);
    display.printf("3");
    display.display();
    delay(1000);
    display.fillRect(SCREEN_WIDTH / 2 - 5, SCREEN_HEIGHT / 2 + 10, 30, 30, BLACK);
    display.setCursor(SCREEN_WIDTH / 2 - 5, SCREEN_HEIGHT / 2 + 20);
    display.printf("2");
    display.display();
    for (int i = 0; i < 10; i++)
    {
        Skip_Test_Loop();
        delay(100);
    }
    display.fillRect(SCREEN_WIDTH / 2 - 5, SCREEN_HEIGHT / 2 + 10, 30, 30, BLACK);
    display.setCursor(SCREEN_WIDTH / 2 - 4, SCREEN_HEIGHT / 2 + 20);
    display.printf("1");
    display.display();
    for (int i = 0; i < 10; i++)
    {
        Skip_Test_Loop();
        delay(100);
    }
}

void GFX_Print_Finish(void)
{
    display.fillScreen(BLACK);
    display.setFont(&FreeSans9pt7b);
    display.setTextSize(1);

    display.setCursor(SCREEN_WIDTH / 4 + 5, SCREEN_HEIGHT / 2 - 15);
    display.printf("Finish");
}

void GFX_Print_Vibration_Info(bool mode)
{
    display.fillScreen(BLACK);
    display.setFont(NULL);
    display.setTextSize(1);
    display.setCursor(23, 5);
    display.printf("Vibration Info");

    display.setFont(&FreeMonoBold9pt7b);
    display.setTextSize(2);

    if (mode == true)
    {
        display.setCursor(40, 50);
        display.printf("ON");
    }
    else
    {
        display.setCursor(30, 50);
        display.printf("OFF");
    }
}

void SX12xx_Walkie_Talkie_Loop()
{
    if (Lora_Send_Data_Stream.size() >= LORA_TRANSMISSION_DATA_SIZE) // 流读取判断
    {
        // 存储数据
        memcpy(&Send_Package[LORA_TRANSMISSION_HEAD_SIZE], Lora_Send_Data_Stream.data(),
               LORA_TRANSMISSION_DATA_SIZE);

        // 删除已经存储的数据
        Lora_Send_Data_Stream.erase(Lora_Send_Data_Stream.begin(),
                                    Lora_Send_Data_Stream.begin() + LORA_TRANSMISSION_DATA_SIZE);

        Serial.println("[SX12xx] Sending another packet ... ");

        radio.transmit(Send_Package,
                       LORA_TRANSMISSION_HEAD_SIZE + LORA_TRANSMISSION_DATA_SIZE);
    }

    // check if the previous operation finished
    if (Radio_Operation_Flag)
    {
        Radio_Operation_Flag = false;

        if (radio.readData(Receive_Package,
                           LORA_TRANSMISSION_HEAD_SIZE + LORA_TRANSMISSION_DATA_SIZE) == RADIOLIB_ERR_NONE)
        {
            if ((Receive_Package[0] == 'M') &&
                (Receive_Package[1] == 'A') &&
                (Receive_Package[2] == 'C') &&
                (Receive_Package[3] == ':'))
            {
                // uint64_t temp_mac =
                //     ((uint64_t)Receive_Package[4] << 56) |
                //     ((uint64_t)Receive_Package[5] << 48) |
                //     ((uint64_t)Receive_Package[6] << 40) |
                //     ((uint64_t)Receive_Package[7] << 32) |
                //     ((uint64_t)Receive_Package[8] << 24) |
                //     ((uint64_t)Receive_Package[9] << 16) |
                //     ((uint64_t)Receive_Package[10] << 8) |
                //     (uint64_t)Receive_Package[11];

                uint64_t temp_mac = 0;
                for (size_t i = 0; i < 8; ++i)
                {
                    temp_mac |= (static_cast<uint64_t>(Receive_Package[4 + i]) << (56 - i * 8));
                }

                if (temp_mac != Local_MAC)
                {
                    // packet was successfully received
                    Serial.println("[SX12xx] Received packet!");

                    // print data of the packet
                    // for (int i = 0; i < LORA_TRANSMISSION_HEAD_SIZE; i++)
                    // {
                    //     Serial.printf("Receive_Package[%d]: %#X\n", i, Receive_Package[i]);
                    // }

                    // for (int i = 0; i < LORA_TRANSMISSION_HEAD_SIZE; i++)
                    // {
                    //     Serial.printf("Send_Package[%d]: %#X\n", i, Send_Package[i]);
                    // }

                    // print RSSI (Received Signal Strength Indicator)
                    Serial.print("[SX12xx] RSSI:\t\t");
                    Serial.print(radio.getRSSI());
                    Serial.println(" dBm");
                    Lora_Receive_RSSI_Value = radio.getRSSI();

                    // print SNR (Signal-to-Noise Ratio)
                    Serial.print("[SX12xx] SNR:\t\t");
                    Serial.print(radio.getSNR());
                    Serial.println(" dB");
                    Lora_Receive_SNR_Value = radio.getSNR();

                    Lora_Receive_MAC = temp_mac;

                    radio.startReceive();

                    CycleTime_2 = millis() + 10000; // 看门狗
                    Lora_Info_Refresh_Flag = true;
                }

                // 写入Lora流中
                const auto current_buf_size = Lora_Receive_Data_Stream.size();

                // 调整容量
                Lora_Receive_Data_Stream.resize(current_buf_size + LORA_TRANSMISSION_DATA_SIZE);

                // 存储数据
                memcpy(Lora_Receive_Data_Stream.data() + current_buf_size,
                       &Receive_Package[LORA_TRANSMISSION_HEAD_SIZE], LORA_TRANSMISSION_DATA_SIZE);
            }
        }
    }
    // Serial.printf("IIS_Read_Data_Stream Size: %d\n", IIS_Read_Data_Stream.size());
    // Serial.printf("IIS_Write_Data_Stream Size: %d\n", IIS_Write_Data_Stream.size());
    // Serial.printf("Lora_Send_Data_Stream Size: %d\n", Lora_Send_Data_Stream.size());
    // Serial.printf("Lora_Receive_Data_Stream Size: %d\n", Lora_Receive_Data_Stream.size());
    delay(2); // xTask time
}

void SD_Test_Loop(void)
{
    Serial.println("Detecting SD card");

    if (SD.begin(SD_CS, SPI_2) == false) // SD boots
    {
        Serial.println("SD bus initialization failed !");
        display.printf("SD bus init failed!\n");

        display.display();
    }
    else
    {
        Serial.println("SD bus initialization successful !");
        display.printf("SD bus init successful!\n");
        display.display();

        uint8_t card_type = SD.cardType();
        uint64_t card_size = SD.cardSize() / (1024 * 1024);
        // uint8_t num_sectors = SD.numSectors();
        switch (card_type)
        {
        case CARD_NONE:
            Serial.println("No SD card attached");
            display.printf("No SD card attached\n");
            display.printf("SD card test failed\n");
            display.display();

            break;
        case CARD_MMC:
            Serial.print("SD Card Type: ");
            Serial.println("MMC");
            Serial.printf("SD Card Size: %lluMB\n", card_size);
            display.printf("SD Card Type:MMC\nSD Card Size:%lluMB\n", card_size);
            display.printf("SD card test successful\n");
            display.display();

            break;
        case CARD_SD:
            Serial.print("SD Card Type: ");
            Serial.println("SDSC");
            Serial.printf("SD Card Size: %lluMB\n", card_size);
            display.printf("SD Card Type:SDSC\nSD Card Size:%lluMB\n", card_size);
            display.printf("SD card tested successful\n");
            display.display();

            break;
        case CARD_SDHC:
            Serial.print("SD Card Type: ");
            Serial.println("SDHC");
            Serial.printf("SD Card Size: %lluMB\n", card_size);
            display.printf("SD Card Type:SDHC\nSD Card Size:%lluMB\n", card_size);
            display.printf("SD card tested successful\n");
            display.display();

            break;
        default:
            Serial.println("UNKNOWN");
            display.printf("UNKNOWN");
            display.printf("SD card test failed\n");
            display.display();

            break;
        }
    }
    SD.end();
}

void RTC_Test_Loop(void)
{
    if (PCF85063_Initialization_Flag == false)
    {
        Serial.println("RTC initialization failed !");
        display.printf("RTC init failed!\n");
    }
    else
    {
        Serial.println("RTC initialization successful !");
        display.printf("RTC init successful!\n\n");

        display.printf("  Weekday:%s\n",
                       PCF85063->IIC_Read_Device_State(PCF85063->Arduino_IIC_RTC::Status_Information::RTC_WEEKDAYS_DATA).c_str());

        display.printf("  Year:%d\n",
                       (int32_t)PCF85063->IIC_Read_Device_Value(PCF85063->Arduino_IIC_RTC::Value_Information::RTC_YEARS_DATA));

        display.printf("  Date:%d.%d\n",
                       (int32_t)PCF85063->IIC_Read_Device_Value(PCF85063->Arduino_IIC_RTC::Value_Information::RTC_MONTHS_DATA),
                       (int32_t)PCF85063->IIC_Read_Device_Value(PCF85063->Arduino_IIC_RTC::Value_Information::RTC_DAYS_DATA));

        display.printf("  Time:%d:%d:%d\n",
                       (int32_t)PCF85063->IIC_Read_Device_Value(PCF85063->Arduino_IIC_RTC::Value_Information::RTC_HOURS_DATA),
                       (int32_t)PCF85063->IIC_Read_Device_Value(PCF85063->Arduino_IIC_RTC::Value_Information::RTC_MINUTES_DATA),
                       (int32_t)PCF85063->IIC_Read_Device_Value(PCF85063->Arduino_IIC_RTC::Value_Information::RTC_SECONDS_DATA));
    }

    display.display();
}

void Original_Test_1()
{
    display.fillScreen(WHITE);
    display.display();

    delay(2000);

    display.fillScreen(BLACK);
    display.display();

    delay(2000);

    GFX_Print_Finish();
    display.display();
}

void Original_Test_2()
{
    ledcAttachPin(VIBRATINO_MOTOR_PWM, 2);
    ledcSetup(2, 12000, 8);
    ledcWrite(2, 0);
    GFX_Print_Vibration_Info(false);

    display.display();
}

void Original_Test_3()
{
#if defined T3_S3_MVSRBoard_V1_0
    pinMode(MSM261_EN, OUTPUT);
    digitalWrite(MSM261_EN, HIGH);

    IIS->end();

    if (IIS->begin(i2s_mode_t::I2S_MODE_MASTER, ad_iis_data_mode_t::AD_IIS_DATA_IN, i2s_channel_fmt_t::I2S_CHANNEL_FMT_ONLY_RIGHT,
                   IIS_DATA_BIT, IIS_SAMPLE_RATE) == false)
    {
        Serial.println("MSM261 initialization fail");
        delay(2000);
    }
    else
    {
        Serial.println("MSM261 initialization successfully");
    }

#elif defined T3_S3_MVSRBoard_V1_1
    pinMode(MP34DT05TR_EN, OUTPUT);
    digitalWrite(MP34DT05TR_EN, LOW);

    IIS->end();

    if (IIS->begin(i2s_mode_t::I2S_MODE_PDM, ad_iis_data_mode_t::AD_IIS_DATA_IN, i2s_channel_fmt_t::I2S_CHANNEL_FMT_ONLY_RIGHT,
                   IIS_DATA_BIT, IIS_SAMPLE_RATE) == false)
    {
        Serial.println("MP34DT05TR initialization fail");
        delay(2000);
    }
    else
    {
        Serial.println("MP34DT05TR initialization successfully");
    }

#else
#error "Unknown macro definition. Please select the correct macro definition."
#endif

    display.fillScreen(BLACK);
    display.setTextSize(1);

    display.setCursor(20, 5);
    display.printf("Microphone Info");
}

void Original_Test_4()
{
    Wifi_STA_Test();

    delay(2000);

    pinMode(MAX98357A_SD_MODE, OUTPUT);
    digitalWrite(MAX98357A_SD_MODE, HIGH);

    audio.setPinout(MAX98357A_BCLK, MAX98357A_LRCLK, MAX98357A_DATA);
    audio.setVolume(3); // 0...21,Volume setting

    if (Wifi_Connection_Flag == true)
    {
        // Wifi_Music_Test();
        SD_Music_Test();
    }
    else
    {
        SD_Music_Test();
    }
}

void Original_Test_5()
{
#if defined T3_S3_MVSRBoard_V1_0
    pinMode(MSM261_EN, OUTPUT);
    digitalWrite(MSM261_EN, HIGH);

    IIS->end();

    if (IIS->begin(i2s_mode_t::I2S_MODE_MASTER, ad_iis_data_mode_t::AD_IIS_DATA_IN, i2s_channel_fmt_t::I2S_CHANNEL_FMT_ONLY_RIGHT,
                   IIS_DATA_BIT, IIS_SAMPLE_RATE) == false)
    {
        Serial.println("MSM261 initialization fail");
        delay(2000);
    }
    else
    {
        Serial.println("MSM261 initialization successfully");
    }

#elif defined T3_S3_MVSRBoard_V1_1
    pinMode(MP34DT05TR_EN, OUTPUT);
    digitalWrite(MP34DT05TR_EN, LOW);

    IIS->end();

    if (IIS->begin(i2s_mode_t::I2S_MODE_PDM, ad_iis_data_mode_t::AD_IIS_DATA_IN, i2s_channel_fmt_t::I2S_CHANNEL_FMT_ONLY_RIGHT,
                   IIS_DATA_BIT, IIS_SAMPLE_RATE) == false)
    {
        Serial.println("MP34DT05TR initialization fail");
        delay(2000);
    }
    else
    {
        Serial.println("MP34DT05TR initialization successfully");
    }

#else
#error "Unknown macro definition. Please select the correct macro definition."
#endif

    pinMode(MAX98357A_SD_MODE, OUTPUT);
    digitalWrite(MAX98357A_SD_MODE, HIGH);

    MAX98357A->end();
    if (MAX98357A->begin(i2s_mode_t::I2S_MODE_MASTER, ad_iis_data_mode_t::AD_IIS_DATA_OUT, i2s_channel_fmt_t::I2S_CHANNEL_FMT_ONLY_RIGHT,
                         IIS_DATA_BIT, IIS_SAMPLE_RATE) == false)
    {
        Serial.println("MAX98357A initialization fail");
        delay(2000);
    }
    else
    {
        Serial.println("MAX98357A initialization successfully");
    }

    SPI.setFrequency(16000000);
    SPI.begin(LORA_SCLK, LORA_MISO, LORA_MOSI);
    int state = -1;

#if defined(T3_S3_SX1262)
    // state = radio.beginFSK();
    state = radio.begin();
#endif

#if defined(T3_S3_SX1280) || defined(T3_S3_SX1280PA) || defined(T3_S3_SX1276) || defined(T3_S3_SX1278)
    state = radio.begin();
#endif

    display.fillScreen(BLACK);
    display.setTextSize(1);

    if (state == RADIOLIB_ERR_NONE)
    {
        Serial.println("SX12xx initialization successful");

        Radio_Initialization_Flag = true;

#ifdef T3_S3_SX1262
        radio.setFrequency(850.0);
        radio.setBitRate(100.0);
        radio.setBandwidth(500.0);
        radio.setCurrentLimit(140);
        radio.setOutputPower(22);
#endif
#ifdef T3_S3_SX1276
        radio.setFrequency(868.1);
        radio.setBandwidth(500.0);

        radio.setCurrentLimit(240);
        radio.setOutputPower(17);
#endif
#ifdef T3_S3_SX1278
        radio.setFrequency(433.1);
        radio.setBandwidth(500.0);

        radio.setCurrentLimit(240);
        radio.setOutputPower(17);
#endif
#ifdef T3_S3_SX1280
        radio.setFrequency(2400.1);
        radio.setBandwidth(1625.0);

        radio.setOutputPower(13);
#endif
#ifdef T3_S3_SX1280PA
        radio.setFrequency(2400.1);
        radio.setBandwidth(1625.0);

        radio.setOutputPower(3);
#endif
        radio.setSpreadingFactor(9);
        radio.setCodingRate(7);
        radio.setSyncWord(RADIOLIB_SX126X_SYNC_WORD_PRIVATE);
        radio.setCRC(false);

        display.setFont(NULL);
        display.setCursor(30, 5);

#ifdef T3_S3_SX1262
        display.print("SX1262 Info");

        display.setFont(&Org_01);
        display.setCursor(0, 20);

        display.print("MODE:LORA");
        display.setCursor(0, 26);
        display.print("F:850.0MHz");
        display.setCursor(0, 33);
        display.print("B:500kHz");
        display.setCursor(0, 40);
        display.print("O:22dBm");
#endif
#ifdef T3_S3_SX1276
        display.print("SX1276 Info");

        display.setFont(&Org_01);
        display.setCursor(0, 20);
        display.print("MODE:LORA");
        display.setCursor(0, 26);
        display.print("F:868.1MHz");
        display.setCursor(0, 33);
        display.print("B:500kHz");
        display.setCursor(0, 40);
        display.print("O:17dBm");
#endif
#ifdef T3_S3_SX1278
        display.print("SX1278 Info");

        display.setFont(&Org_01);
        display.setCursor(0, 20);
        display.print("MODE:LORA");
        display.setCursor(0, 26);
        display.print("F:433.1MHz");
        display.setCursor(0, 33);
        display.print("B:500kHz");
        display.setCursor(0, 40);
        display.print("O:17dBm");
#endif
#ifdef T3_S3_SX1280
        display.print("SX1280 Info");

        display.setFont(&Org_01);
        display.setCursor(0, 20);
        display.print("MODE:LORA");
        display.setCursor(0, 26);
        display.print("F:2400.1MHz");
        display.setCursor(0, 33);
        display.print("B:1625.0kHz");
        display.setCursor(0, 40);
        display.print("O:13dBm");
#endif
#ifdef T3_S3_SX1280PA
        display.print("SX1280PA Info");

        display.setFont(&Org_01);
        display.setCursor(0, 20);
        display.print("MODE:LORA");
        display.setCursor(0, 26);
        display.print("F:2400.1MHz");
        display.setCursor(0, 33);
        display.print("B:1625.0kHz");
        display.setCursor(0, 40);
        display.print("O:3dBm");
#endif

        display.setCursor(0, 53);
        display.printf("L_MAC:%llu", Local_MAC);

        GFX_Print_SX12xx_Walkie_Talkie_Info_Loop();

        // 恢复之前被挂起的任务
        vTaskResume(Codec2_Task_Handle);
        vTaskResume(MAX_Play_Task_Handle);

        radio.startReceive(); // 开始接收
    }
    else
    {
        Radio_Initialization_Flag = false;
        Serial.printf("SX12xx initialization failed Error code: %d\n", state);

        display.setFont(NULL);
        display.setCursor(5, 5);
        display.print("SX12xx initialization failed");
    }

    display.display();
}

void Original_Test_6()
{
    SPI_2.setFrequency(16000000);
    SPI_2.begin(SD_SCLK, SD_MISO, SD_MOSI);

    display.fillScreen(BLACK);
    display.setTextSize(1);

    display.setCursor(40, 5);
    display.printf("SD Info");
}

void Original_Test_7()
{
    if (PCF85063->begin() == false)
    {
        Serial.println("PCF85063 initialization fail");
        PCF85063_Initialization_Flag = false;
        delay(2000);
    }
    else
    {
        Serial.println("PCF85063 initialization successfully");

        // 设置时间格式为24小时制
        PCF85063->IIC_Write_Device_State(PCF85063->Arduino_IIC_RTC::Device::RTC_CLOCK_TIME_FORMAT,
                                         PCF85063->Arduino_IIC_RTC::Device_Mode::RTC_CLOCK_TIME_FORMAT_24);

        // 关闭时钟输出
        PCF85063->IIC_Write_Device_State(PCF85063->Arduino_IIC_RTC::Device::RTC_CLOCK_OUTPUT_VALUE,
                                         PCF85063->Arduino_IIC_RTC::Device_Mode::RTC_CLOCK_OUTPUT_OFF);

        // 开启RTC
        PCF85063->IIC_Write_Device_State(PCF85063->Arduino_IIC_RTC::Device::RTC_CLOCK_RTC,
                                         PCF85063->Arduino_IIC_RTC::Device_State::RTC_DEVICE_ON);

        PCF85063_Initialization_Flag = true;
    }

    display.fillScreen(BLACK);
    display.setTextSize(1);

    display.setCursor(37, 5);
    display.printf("RTC Info");
}

void Original_Test_Loop()
{
    GFX_Print_TEST("OLED screen test");
    if (Skip_Current_Test == false)
    {
        Original_Test_1();

        while (1)
        {
            bool temp = false;

            if (Key_Scanning() == true)
            {
                switch (Button_Triggered_OP.current_state)
                {
                case Button_Triggered_OP.gesture::SINGLE_CLICK:
                    Serial.println("Key triggered: SINGLE_CLICK");

                    // delay(1000);
                    break;
                case Button_Triggered_OP.gesture::DOUBLE_CLICK:
                    Serial.println("Key triggered: DOUBLE_CLICK");

                    GFX_Print_TEST("OLED screen test");
                    Original_Test_1();

                    // delay(1000);
                    break;
                case Button_Triggered_OP.gesture::LONG_PRESS:
                    Serial.println("Key triggered: LONG_PRESS");
                    temp = true;
                    // delay(1000);
                    break;

                default:
                    break;
                }
            }

            if ((temp == true) || (Skip_Current_Test == true))
            {
                Skip_Current_Test = false;
                break;
            }
        }
    }
    else
    {
        Skip_Current_Test = false;
    }

    GFX_Print_TEST("Vibration motor test");
    if (Skip_Current_Test == false)
    {
        Original_Test_2();

        while (1)
        {
            bool temp = false;

            if (Key_Scanning() == true)
            {
                switch (Button_Triggered_OP.current_state)
                {
                case Button_Triggered_OP.gesture::SINGLE_CLICK:
                    Serial.println("Key triggered: SINGLE_CLICK");

                    Vibration_Start_Flag = !Vibration_Start_Flag;

                    if (Vibration_Start_Flag == true)
                    {
                        ledcWrite(2, 255);
                    }
                    else
                    {
                        ledcWrite(2, 0);
                    }

                    GFX_Print_Vibration_Info(Vibration_Start_Flag);

                    display.display();

                    // delay(300);
                    break;
                case Button_Triggered_OP.gesture::DOUBLE_CLICK:
                    Serial.println("Key triggered: DOUBLE_CLICK");

                    GFX_Print_TEST("Vibration motor test");
                    Original_Test_2();

                    // delay(1000);
                    break;
                case Button_Triggered_OP.gesture::LONG_PRESS:
                    Serial.println("Key triggered: LONG_PRESS");
                    temp = true;
                    // delay(1000);
                    break;

                default:
                    break;
                }
            }

            if ((temp == true) || (Skip_Current_Test == true))
            {
                Skip_Current_Test = false;
                ledcWrite(2, 0);
                Vibration_Start_Flag = false;
                break;
            }
        }
    }
    else
    {
        Skip_Current_Test = false;
    }

    GFX_Print_TEST("SD Test");
    if (Skip_Current_Test == false)
    {
        Original_Test_6();

        while (1)
        {
            bool temp = false;

            if (millis() > CycleTime)
            {
                display.fillRect(0, 14, 128, 50, BLACK);
                display.setFont(NULL);
                display.setTextSize(1);
                display.setCursor(0, 15);

                SD_Test_Loop();
                CycleTime = millis() + 1000;
            }

            if (Key_Scanning() == true)
            {
                switch (Button_Triggered_OP.current_state)
                {
                case Button_Triggered_OP.gesture::SINGLE_CLICK:
                    Serial.println("Key triggered: SINGLE_CLICK");

                    // delay(300);
                    break;
                case Button_Triggered_OP.gesture::DOUBLE_CLICK:
                    Serial.println("Key triggered: DOUBLE_CLICK");

                    GFX_Print_TEST("SD Test");
                    Original_Test_6();

                    // delay(1000);
                    break;
                case Button_Triggered_OP.gesture::LONG_PRESS:
                    Serial.println("Key triggered: LONG_PRESS");
                    temp = true;
                    // delay(1000);
                    break;

                default:
                    break;
                }
            }

            if ((temp == true) || (Skip_Current_Test == true))
            {
                Skip_Current_Test = false;
                break;
            }
        }
    }
    else
    {
        Skip_Current_Test = false;
    }

    GFX_Print_TEST("RTC Test");
    if (Skip_Current_Test == false)
    {
        Original_Test_7();

        while (1)
        {
            bool temp = false;

            if (millis() > CycleTime)
            {
                display.fillRect(0, 14, 128, 50, BLACK);
                display.setFont(NULL);
                display.setTextSize(1);
                display.setCursor(0, 15);

                RTC_Test_Loop();
                CycleTime = millis() + 1000;
            }

            if (Key_Scanning() == true)
            {
                switch (Button_Triggered_OP.current_state)
                {
                case Button_Triggered_OP.gesture::SINGLE_CLICK:
                    Serial.println("Key triggered: SINGLE_CLICK");

                    // 重置时间
                    //  关闭RTC
                    PCF85063->IIC_Write_Device_State(PCF85063->Arduino_IIC_RTC::Device::RTC_CLOCK_RTC,
                                                     PCF85063->Arduino_IIC_RTC::Device_State::RTC_DEVICE_OFF);
                    // 时钟传感器设置秒
                    PCF85063->IIC_Write_Device_Value(PCF85063->Arduino_IIC_RTC::Device_Value::RTC_SET_SECOND_DATA,
                                                     58);
                    // 时钟传感器设置分
                    PCF85063->IIC_Write_Device_Value(PCF85063->Arduino_IIC_RTC::Device_Value::RTC_SET_MINUTE_DATA,
                                                     59);
                    // 时钟传感器设置时
                    PCF85063->IIC_Write_Device_Value(PCF85063->Arduino_IIC_RTC::Device_Value::RTC_SET_HOUR_DATA,
                                                     23);
                    // 时钟传感器设置天
                    PCF85063->IIC_Write_Device_Value(PCF85063->Arduino_IIC_RTC::Device_Value::RTC_SET_DAY_DATA,
                                                     31);
                    // 时钟传感器设置月
                    PCF85063->IIC_Write_Device_Value(PCF85063->Arduino_IIC_RTC::Device_Value::RTC_SET_MONTH_DATA,
                                                     12);
                    // 时钟传感器设置
                    PCF85063->IIC_Write_Device_Value(PCF85063->Arduino_IIC_RTC::Device_Value::RTC_SET_YEAR_DATA,
                                                     99);
                    // 开启RTC
                    PCF85063->IIC_Write_Device_State(PCF85063->Arduino_IIC_RTC::Device::RTC_CLOCK_RTC,
                                                     PCF85063->Arduino_IIC_RTC::Device_State::RTC_DEVICE_ON);

                    // delay(300);
                    break;
                case Button_Triggered_OP.gesture::DOUBLE_CLICK:
                    Serial.println("Key triggered: DOUBLE_CLICK");

                    GFX_Print_TEST("RTC Test");
                    Original_Test_7();

                    // delay(1000);
                    break;
                case Button_Triggered_OP.gesture::LONG_PRESS:
                    Serial.println("Key triggered: LONG_PRESS");
                    temp = true;
                    // delay(1000);
                    break;

                default:
                    break;
                }
            }

            if ((temp == true) || (Skip_Current_Test == true))
            {
                Skip_Current_Test = false;
                break;
            }
        }
    }
    else
    {
        Skip_Current_Test = false;
    }

    GFX_Print_TEST("Microphone Test");
    if (Skip_Current_Test == false)
    {
        Original_Test_3();

        while (1)
        {
            bool temp = false;

            if (millis() > CycleTime)
            {
                short iis_read_buf[sizeof(short) * 100];

                if (IIS->IIS_Read_Data(iis_read_buf, sizeof(short) * 100) == true)
                {
                    display.fillRect(0, 20, 128, 40, BLACK);

                    display.setCursor(10, 20);
                    display.printf("Data:%d", iis_read_buf[0]);

                    display.display();

                    // for (int i = 0; i < 100; i++)
                    // {
                    //     Serial.printf("Data[%d]:%d\n", i, iis_read_buf[i]);
                    // }
                }

                CycleTime = millis() + 50;
            }

            if (Key_Scanning() == true)
            {
                switch (Button_Triggered_OP.current_state)
                {
                case Button_Triggered_OP.gesture::SINGLE_CLICK:
                    Serial.println("Key triggered: SINGLE_CLICK");

                    // delay(300);
                    break;
                case Button_Triggered_OP.gesture::DOUBLE_CLICK:
                    Serial.println("Key triggered: DOUBLE_CLICK");

                    GFX_Print_TEST("Microphone Test");
                    Original_Test_3();

                    // delay(1000);
                    break;
                case Button_Triggered_OP.gesture::LONG_PRESS:
                    Serial.println("Key triggered: LONG_PRESS");
                    temp = true;
                    // delay(1000);
                    break;

                default:
                    break;
                }
            }

            if ((temp == true) || (Skip_Current_Test == true))
            {
                Skip_Current_Test = false;

#if defined T3_S3_MVSRBoard_V1_0
                digitalWrite(MSM261_EN, LOW);

#elif defined T3_S3_MVSRBoard_V1_1
                digitalWrite(MP34DT05TR_EN, HIGH);
#else
#error "Unknown macro definition. Please select the correct macro definition."
#endif
                break;
            }
        }
    }
    else
    {
        Skip_Current_Test = false;
    }

    GFX_Print_TEST("WIFI SD music test");
    if (Skip_Current_Test == false)
    {
        Original_Test_4();

        while (1)
        {
            bool temp = false;

            if (Music_Start_Playing_Flag == true)
            {
                audio.loop();
            }

            if (Key_Scanning() == true)
            {
                switch (Button_Triggered_OP.current_state)
                {
                case Button_Triggered_OP.gesture::SINGLE_CLICK:
                    Serial.println("Key triggered: SINGLE_CLICK");

                    Volume_Change_Flag = !Volume_Change_Flag;

                    if (Volume_Change_Flag == 0)
                    {
                        audio.setVolume(3);
                    }
                    else
                    {
                        audio.setVolume(21);
                    }

                    GFX_Print_Music_Volume_Value();

                    // delay(300);
                    break;
                case Button_Triggered_OP.gesture::DOUBLE_CLICK:
                    Serial.println("Key triggered: DOUBLE_CLICK");

                    GFX_Print_TEST("WIFI SD music test");
                    Original_Test_4();

                    // delay(1000);
                    break;
                case Button_Triggered_OP.gesture::LONG_PRESS:
                    Serial.println("Key triggered: LONG_PRESS");
                    temp = true;
                    // delay(1000);
                    break;

                default:
                    break;
                }
            }

            if ((temp == true) || (Skip_Current_Test == true))
            {
                Skip_Current_Test = false;
                digitalWrite(MAX98357A_SD_MODE, LOW);
                break;
            }
        }
    }
    else
    {
        Skip_Current_Test = false;
    }

#ifdef T3_S3_SX1262
    GFX_Print_TEST("SX1262 walkie talkie test");
#endif
#ifdef T3_S3_SX1276
    GFX_Print_TEST("SX1276 walkie talkie test");
#endif
#ifdef T3_S3_SX1278
    GFX_Print_TEST("SX1278 walkie talkie test");
#endif
#ifdef T3_S3_SX1280
    GFX_Print_TEST("SX1280 walkie talkie test");
#endif
#ifdef T3_S3_SX1280PA
    GFX_Print_TEST("SX1280PA walkie talkie test");
#endif
    if (Skip_Current_Test == false)
    {
        Original_Test_5();

        while (1)
        {
            bool temp = false;

            // int16_t iis_buf[sizeof(short) * 100] = {0};
            // std::vector<short> output_buf;

            // if (IIS->IIS_Read_Data(iis_buf, sizeof(short) * 100) == true)
            // {
            //     // 单声道转双声道
            //     IIS_Dual_Conversion(iis_buf, &output_buf, 100, 1.0);

            //     if (MAX98357A->IIS_Write_Data(output_buf.data(), 2 * sizeof(short) * 100) == true)
            //     {
            //         // Serial.printf("MAX98357A played successfully\n");
            //     }
            // }
            if (Lora_Transmission_Mode == 0) // 如果为数据接收模式
            {
                if (millis() > CycleTime_2) // 看门狗
                {
                    Lora_Info_Refresh_Flag = true;

                    Lora_Receive_RSSI_Value = 0;
                    Lora_Receive_SNR_Value = 0;
                    Lora_Receive_MAC = 0;
                }
            }

            if ((millis() > CycleTime) && (Lora_Info_Refresh_Flag == true) && (Radio_Initialization_Flag == true))
            {
                GFX_Print_SX12xx_Walkie_Talkie_Info_Loop();
                Lora_Info_Refresh_Flag = false;
                CycleTime = millis() + 1000;
            }

            if (Radio_Initialization_Flag == true)
            {
                SX12xx_Walkie_Talkie_Loop();
            }

            if (Key_Scanning() == true)
            {
                switch (Button_Triggered_OP.current_state)
                {
                case Button_Triggered_OP.gesture::SINGLE_CLICK:
                    Serial.println("Key triggered: SINGLE_CLICK");

                    if (Radio_Initialization_Flag == true)
                    {
                        Lora_Transmission_Mode = !Lora_Transmission_Mode;
                        Lora_Info_Refresh_Flag = true;

                        if (Lora_Transmission_Mode == 0)
                        {
#if defined(T3_S3_SX1280) || defined(T3_S3_SX1280PA) || defined(T3_S3_SX1276) || defined(T3_S3_SX1278)
                            radio.begin();
#endif
#if defined(T3_S3_SX1262)
                            // radio.beginFSK();
                            radio.begin();
#endif
#ifdef T3_S3_SX1262
                            radio.setFrequency(850.0);
                            radio.setBitRate(100.0);
                            radio.setBandwidth(500.0);
                            radio.setCurrentLimit(140);
                            radio.setOutputPower(22);
#endif
#ifdef T3_S3_SX1276
                            radio.setFrequency(868.1);
                            radio.setBandwidth(500.0);
                            radio.setCurrentLimit(240);
                            radio.setOutputPower(17);
#endif
#ifdef T3_S3_SX1278
                            radio.setFrequency(433.1);
                            radio.setBandwidth(500.0);
                            radio.setCurrentLimit(240);
                            radio.setOutputPower(17);
#endif
#ifdef T3_S3_SX1280
                            radio.setFrequency(2400.1);
                            radio.setBandwidth(1625.0);
                            radio.setOutputPower(13);
#endif
#ifdef T3_S3_SX1280PA
                            radio.setFrequency(2400.1);
                            radio.setBandwidth(1625.0);

                            radio.setOutputPower(3);
#endif
                            radio.setSpreadingFactor(9);
                            radio.setCodingRate(7);
                            radio.setSyncWord(RADIOLIB_SX126X_SYNC_WORD_PRIVATE);
                            radio.setCRC(false);

                            radio.startReceive();
                        }
                        IIS_Read_Data_Stream.clear();
                        IIS_Write_Data_Stream.clear();
                        Lora_Send_Data_Stream.clear();
                        Lora_Receive_Data_Stream.clear();
                    }

                    // delay(300);
                    break;
                case Button_Triggered_OP.gesture::DOUBLE_CLICK:
                    Serial.println("Key triggered: DOUBLE_CLICK");

#ifdef T3_S3_SX1262
                    GFX_Print_TEST("SX1262 walkie talkie test");
#endif
#ifdef T3_S3_SX1276
                    GFX_Print_TEST("SX1276 walkie talkie test");
#endif
#ifdef T3_S3_SX1278
                    GFX_Print_TEST("SX1278 walkie talkie test");
#endif
#ifdef T3_S3_SX1280
                    GFX_Print_TEST("SX1280 walkie talkie test");
#endif
#ifdef T3_S3_SX1280PA
                    GFX_Print_TEST("SX1280PA walkie talkie test");
#endif
                    Original_Test_5();

                    // delay(1000);
                    break;
                case Button_Triggered_OP.gesture::LONG_PRESS:
                    Serial.println("Key triggered: LONG_PRESS");
                    temp = true;
                    // delay(1000);
                    break;

                default:
                    break;
                }
            }

            if ((temp == true) || (Skip_Current_Test == true))
            {
                Skip_Current_Test = false;

                // 暂停任务
                vTaskSuspend(Codec2_Task_Handle);
                vTaskSuspend(MAX_Play_Task_Handle);

#if defined T3_S3_MVSRBoard_V1_0
                digitalWrite(MSM261_EN, LOW);

#elif defined T3_S3_MVSRBoard_V1_1
                digitalWrite(MP34DT05TR_EN, HIGH);
#else
#error "Unknown macro definition. Please select the correct macro definition."
#endif
                digitalWrite(MAX98357A_SD_MODE, LOW);
                break;
            }
        }
    }
    else
    {
        Skip_Current_Test = false;
    }
}

void Codec2_Task(void *parameter)
{
    struct CODEC2 *codec2_state;
    // 麦克风采样次数 1次有8个uint8_t的数据 5次就是40个uint8_t数据
    const uint8_t msm_sample_frequency = LORA_TRANSMISSION_DATA_SIZE / 8;

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
        delay(1);

        if (Lora_Transmission_Mode == 1) // 编码发送
        {
            for (int i = 0; i < msm_sample_frequency; i++)
            {
                if (IIS->IIS_Read_Data(msm_sample_buf, msm_sample_buf_size) == true)
                {
                    // for (int i = 0; i < 60; i++)
                    // {
                    //     Serial.printf("debug1: %d\n", c2_samples_[i]);
                    // }
                    // Serial.println();

                    // uint32_t startTimeEncode = millis();
                    codec2_encode(codec2_state, codec2_compress_buf, msm_sample_buf);
                    // Serial.println("Done encoding, took ms: " + String(millis() - startTimeEncode));

                    // delay(1);

                    // 写入Lora流中
                    const auto current_buf_size = Lora_Send_Data_Stream.size();

                    // 调整容量
                    Lora_Send_Data_Stream.resize(current_buf_size + codec2_compress_size);

                    // 存储数据
                    memcpy(Lora_Send_Data_Stream.data() + current_buf_size, codec2_compress_buf,
                           codec2_compress_size);
                }
            }
            // Serial.printf("size1: %d\n", IIS_Read_Data_Stream.size());
        }

        if (Lora_Receive_Data_Stream.size() >= codec2_compress_size) // 流读取判断
        {
            unsigned char lora_data_buf[codec2_compress_size];
            std::vector<short> output_buf;

            // 存储数据
            memcpy(lora_data_buf, Lora_Receive_Data_Stream.data(), codec2_compress_size);

            // 删除已经存储的数据
            Lora_Receive_Data_Stream.erase(Lora_Receive_Data_Stream.begin(),
                                           Lora_Receive_Data_Stream.begin() + codec2_compress_size);

            codec2_decode(codec2_state, codec2_buf, lora_data_buf);
            // Serial.println("Done decoding, took ms: " + String(millis() - startTimeDecode));

            // IIS_Dual_Conversion(codec2_buf, &output_buf, codec2_sample_size, 5.0);

            // if (MAX98357A->IIS_Write_Data(output_buf.data(), max_sample_buf_size) == true)
            // {
            //     // Serial.printf("MAX98357A played successfully\n");

            //     // for (int i = 0; i < 25; i++)
            //     // {
            //     //     Serial.printf("debug: %d\n", (int16_t)(temp[i + 2] | temp[i + 3] << 8));
            //     // }
            // }

            const auto current_buf_size = IIS_Write_Data_Stream.size();

            // Serial.printf("size0: %d\n", output_buf_size);

            // // 调整容量
            // IIS_Write_Data_Stream.resize(current_buf_size + output_buf.size());

            // // 存储数据
            // memcpy(IIS_Write_Data_Stream.data() + current_buf_size, output_buf.data(),
            //        sizeof(short) * output_buf.size());

            // 调整容量
            IIS_Write_Data_Stream.resize(current_buf_size + codec2_sample_size);

            // 存储数据
            memcpy(IIS_Write_Data_Stream.data() + current_buf_size, codec2_buf,
                   sizeof(short) * codec2_sample_size);
        }
    }
}

void MAX_Play_Task(void *parameter)
{
    const short max_play_size = 320;

    while (1)
    {
        delay(1);

        if (IIS_Write_Data_Stream.size() >= max_play_size) // 流读取判断
        {
            short iis_data_buf[max_play_size];

            // 存储数据
            memcpy(iis_data_buf, IIS_Write_Data_Stream.data(), sizeof(short) * max_play_size);

            // 删除已经存储的数据
            IIS_Write_Data_Stream.erase(IIS_Write_Data_Stream.begin(),
                                        IIS_Write_Data_Stream.begin() + max_play_size);

            if (MAX98357A->IIS_Write_Data(iis_data_buf, sizeof(short) * max_play_size) == true)
            {
                // Serial.printf("MAX98357A played successfully\n");

                // for (int i = 0; i < 25; i++)
                // {
                //     Serial.printf("debug: %d\n", (int16_t)(IIS_Write_Buf[i + 2] | IIS_Write_Buf[i + 3] << 8));
                // }
                // delay(5);
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
    Serial.println("Ciallo");
    Serial.println("[T3-S3-MVSRBoard_" + (String)BOARD_VERSION "][" + (String)SOFTWARE_NAME +
                   "]_firmware_" + (String)SOFTWARE_LASTEDITTIME);

    xTaskCreate(&Codec2_Task, "Codec2_Task", 32000, NULL, 5, &Codec2_Task_Handle);
    xTaskCreate(&MAX_Play_Task, "MAX_Play_Task", 30000, NULL, 5, &MAX_Play_Task_Handle);

    // 暂停任务
    vTaskSuspend(Codec2_Task_Handle);
    vTaskSuspend(MAX_Play_Task_Handle);

    radio.setPacketReceivedAction(Radio_Interrupt);

    Wire.setPins(SCREEN_SDA, SCREEN_SCL);

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println("SSD1306 initialization failed");
        delay(1000);
    }
    else
    {
        Serial.println("SSD1306 initialization successful");
    }

    display.fillScreen(BLACK);
    display.setTextColor(WHITE);
    display.setFont(&FreeSerifBold9pt7b);
    display.setTextSize(1);
    display.setCursor(25, 15);
    display.print("LILYGO");

    display.setFont(NULL);
    display.setCursor(5, 20);
    display.print("Software: " + (String)SOFTWARE_NAME);
    display.setCursor(5, 37);
    display.print("LastEditTime: " + (String)SOFTWARE_LASTEDITTIME);
    display.setCursor(5, 55);
    display.print("Test items: 7");

    display.display();

    delay(3000);

    Original_Test_Loop();
}

void loop()
{
    display.fillScreen(BLACK);
    display.setTextColor(WHITE);
    display.setFont(&FreeSans9pt7b);
    display.setTextSize(1);
    display.setCursor(22, 15);
    display.print("Test Finish");

    display.setFont(NULL);
    display.setCursor(5, 20);
    display.print("Software: " + (String)SOFTWARE_NAME);
    display.setCursor(5, 45);
    display.printf("Running time: %lus", millis() / 1000);

    display.display();

    delay(1000);
}

// optional
void audio_info(const char *info)
{
    Serial.print("info        ");
    Serial.println(info);
}
void audio_id3data(const char *info)
{ // id3 metadata
    Serial.print("id3data     ");
    Serial.println(info);
}
void audio_eof_mp3(const char *info)
{ // end of file
    Serial.print("eof_mp3     ");
    Serial.println(info);
}
void audio_showstation(const char *info)
{
    Serial.print("station     ");
    Serial.println(info);
}
void audio_showstreamtitle(const char *info)
{
    Serial.print("streamtitle ");
    Serial.println(info);
}
void audio_bitrate(const char *info)
{
    Serial.print("bitrate     ");
    Serial.println(info);
}
void audio_commercial(const char *info)
{ // duration in sec
    Serial.print("commercial  ");
    Serial.println(info);
}
void audio_icyurl(const char *info)
{ // homepage
    Serial.print("icyurl      ");
    Serial.println(info);
}
void audio_lasthost(const char *info)
{ // stream URL played
    Serial.print("lasthost    ");
    Serial.println(info);
}
