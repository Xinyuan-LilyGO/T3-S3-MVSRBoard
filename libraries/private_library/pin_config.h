/*
 * @Description: None
 * @version: None
 * @Author: None
 * @Date: 2023-06-05 13:01:59
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2025-03-18 12:01:33
 */
#pragma once

/////////////////////////////////////////////////////////////////////////
// #define T3_S3_MVSRBoard_V1_0
#define T3_S3_MVSRBoard_V1_1
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
#define T3_S3_SX1262
// #define T3_S3_SX1276
// #define T3_S3_SX1278
// #define T3_S3_SX1280
// #define T3_S3_SX1280PA

#ifdef T3_S3_SX1262
#define LORA_DIO1 33
#define LORA_BUSY 34
#endif

#if defined(T3_S3_SX1276) || defined(T3_S3_SX1278)
#define LORA_DIO0 9
#define LORA_DIO1 33
#define LORA_DIO2 34
#define LORA_DIO3 21
#define LORA_DIO4 10
#define LORA_DIO5 36
#endif

#if defined(T3_S3_SX1280) || defined(T3_S3_SX1280PA)
#define LORA_DIO1 9
#define LORA_BUSY 36
#define LORA_TX 10
#define LORA_RX 21
#endif

/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// LORA
#define LORA_CS 7
#define LORA_RST 8
#define LORA_SCLK 5
#define LORA_MOSI 6
#define LORA_MISO 3

// IIC
#define IIC_SDA 42
#define IIC_SCL 45

// SD
#define SD_SCLK 14
#define SD_MISO 2
#define SD_MOSI 11
#define SD_CS 13

// BOOT
#define BOOT_KEY 0

// LED
#define LED_1 37

// Screen SSD1306
#define SCREEN_RST -1
#define SCREEN_SDA 18
#define SCREEN_SCL 17
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C

// Vibratino Motor
#define VIBRATINO_MOTOR_PWM 46

// PCF85063
#define PCF85063_IIC_SDA 42
#define PCF85063_IIC_SCL 45
#define PCF85063_INT 16

#if defined T3_S3_MVSRBoard_V1_0
// MSM261S4030H0R
#define MSM261_EN 35
#define MSM261_BCLK 47
#define MSM261_WS 15
#define MSM261_DATA 48
#elif defined T3_S3_MVSRBoard_V1_1
// MP34DT05TR
#define MP34DT05TR_LRCLK 15
#define MP34DT05TR_DATA 48
#define MP34DT05TR_EN 35
#else
#error "Unknown macro definition. Please select the correct macro definition."
#endif

// MAX98357AETE+T
#define MAX98357A_BCLK 40
#define MAX98357A_LRCLK 41
#define MAX98357A_DATA 39
#define MAX98357A_SD_MODE 38

/////////////////////////////////////////////////////////////////////////
