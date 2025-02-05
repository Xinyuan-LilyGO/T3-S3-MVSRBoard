/*
 * @Description: Sleep
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2024-03-11 10:05:32
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2024-11-04 11:05:19
 * @License: GPL 3.0
 */
#include <SPI.h>
#include <Wire.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "pin_config.h"
#include "Display_Fonts.h"
#include "RadioLib.h"

#define SOFTWARE_NAME "Sleep_Wake_Up"
#define SOFTWARE_LASTEDITTIME "202411041104"
#define SOFTWARE_VERSION "V1.0.0"
#define BOARD_VERSION "V1.0"

#define SLEEP_WAKE_UP_INT GPIO_NUM_0

#define AUTOMATICALLY_ENTER_LIGHT_SLEEP_TIME 10000

struct Button_Triggered_Operator
{
    using gesture = enum {
        NOT_ACTIVE,   // 未激活
        SINGLE_CLICK, // 单击
        DOUBLE_CLICK, // 双击
        LONG_PRESS,   // 长按
    };
    const uint32_t button_number = SLEEP_WAKE_UP_INT;

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

struct Sleep_Operator
{
    using mode = enum {
        NOT_SLEEP,
        LIGHT_SLEEP,
        DEEP_SLEEP,
    };

    size_t cycletime_1 = 0;

    uint8_t current_mode = mode::NOT_SLEEP;
};

// flag to indicate that a packet was sent or received
volatile bool operationDone = false;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, SCREEN_RST);

SX1262 radio = new Module(LORA_CS, LORA_DIO1, LORA_RST, LORA_BUSY, SPI);

Button_Triggered_Operator Button_Triggered_OP;
Sleep_Operator Sleep_OP;

void setFlag(void)
{
    // we sent or received a packet, set the flag
    operationDone = true;
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

void System_Sleep(bool mode)
{
    if (mode == true)
    {
        digitalWrite(MSM261_EN, LOW);
        digitalWrite(MAX98357A_SD_MODE, LOW);

        display.fillScreen(BLACK);
        display.display();
        display.ssd1306_command(0XAE); // Sleep

        radio.sleep();

        digitalWrite(LED_1, LOW);
    }
    else
    {
        Serial.begin(115200);
        Serial.println("Ciallo");
        Serial.println("[T3-S3-MVSRBoard_" + (String)BOARD_VERSION "][" + (String)SOFTWARE_NAME +
                       "]_firmware_" + (String)SOFTWARE_VERSION + "_" + (String)SOFTWARE_LASTEDITTIME);

        pinMode(SLEEP_WAKE_UP_INT, INPUT_PULLUP);

        pinMode(LED_1, OUTPUT);
        digitalWrite(LED_1, HIGH);

        pinMode(MSM261_EN, OUTPUT);
        digitalWrite(MSM261_EN, HIGH);

        pinMode(MAX98357A_SD_MODE, OUTPUT);
        digitalWrite(MAX98357A_SD_MODE, HIGH);

        Wire.setPins(SCREEN_SDA, SCREEN_SCL);

        // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
        while (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
        {
            Serial.println("SSD1306 initialization failed");
            delay(1000);
        }
        Serial.println("SSD1306 initialization successful");
        // initialize SX1262 with default settings
        Serial.println("[SX1262] Initializing ... ");

        SPI.setFrequency(16000000);
        SPI.begin(LORA_SCLK, LORA_MISO, LORA_MOSI);
        // int state = radio.begin();
        int state = radio.beginFSK();
        if (state == RADIOLIB_ERR_NONE)
        {
            Serial.println("success!");
        }
        else
        {
            Serial.print("failed, code ");
            Serial.println(state);
            while (true)
                ;
        }

        // radio.setFrequency(914.9);
        // radio.setFrequency(914.8);
        radio.setFrequency(914.7);
        radio.setBandwidth(500.0);
        radio.setSpreadingFactor(12);
        radio.setCodingRate(8);
        radio.setSyncWord(0xAB);
        radio.setOutputPower(22);
        radio.setCurrentLimit(140);
        radio.setPreambleLength(16);
        radio.setCRC(false);

        // set the function that will be called
        // when new packet is received
        radio.setDio1Action(setFlag);
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");
    Serial.println("[T3-S3-MVSRBoard_" + (String)BOARD_VERSION "][" + (String)SOFTWARE_NAME +
                   "]_firmware_" + (String)SOFTWARE_VERSION + "_" + (String)SOFTWARE_LASTEDITTIME);

    pinMode(SLEEP_WAKE_UP_INT, INPUT_PULLUP);

    pinMode(LED_1, OUTPUT);
    digitalWrite(LED_1, HIGH);

    pinMode(MSM261_EN, OUTPUT);
    digitalWrite(MSM261_EN, HIGH);

    pinMode(MAX98357A_SD_MODE, OUTPUT);
    digitalWrite(MAX98357A_SD_MODE, HIGH);

    Wire.setPins(SCREEN_SDA, SCREEN_SCL);

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    while (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println("SSD1306 initialization failed");
        delay(1000);
    }
    Serial.println("SSD1306 initialization successful");
    // initialize SX1262 with default settings
    Serial.println("[SX1262] Initializing ... ");

    SPI.setFrequency(16000000);
    SPI.begin(LORA_SCLK, LORA_MISO, LORA_MOSI);
    // int state = radio.begin();
    int state = radio.beginFSK();
    if (state == RADIOLIB_ERR_NONE)
    {
        Serial.println("success!");
    }
    else
    {
        Serial.print("failed, code ");
        Serial.println(state);
        while (true)
            ;
    }

    // radio.setFrequency(914.9);
    // radio.setFrequency(914.8);
    radio.setFrequency(914.7);
    radio.setBandwidth(500.0);
    radio.setSpreadingFactor(12);
    radio.setCodingRate(8);
    radio.setSyncWord(0xAB);
    radio.setOutputPower(22);
    radio.setCurrentLimit(140);
    radio.setPreambleLength(16);
    radio.setCRC(false);

    // set the function that will be called
    // when new packet is received
    radio.setDio1Action(setFlag);

    display.setTextColor(WHITE);
    display.setFont(&FreeSerifBold9pt7b);
    display.setTextSize(1);
    display.setCursor(5, 25);
    display.print("Enter light sleep in 10 seconds");

    display.display();

    Serial.println("Enter light sleep in 10 seconds");

    Sleep_OP.cycletime_1 = millis() + AUTOMATICALLY_ENTER_LIGHT_SLEEP_TIME;
}

void loop()
{
    if (millis() > Sleep_OP.cycletime_1)
    {
        if (Sleep_OP.current_mode != Sleep_OP.mode::LIGHT_SLEEP)
        {
            Serial.println("Light Sleep");

            display.fillScreen(BLACK);
            display.setTextSize(1);
            display.setCursor(5, 25);
            display.print("Light Sleep");
            display.display();

            Sleep_OP.current_mode = Sleep_OP.mode::LIGHT_SLEEP;

            delay(1000);

            System_Sleep(true);
        }
    }

    if (Sleep_OP.current_mode == Sleep_OP.mode::LIGHT_SLEEP)
    {
        if (digitalRead(SLEEP_WAKE_UP_INT) == LOW)
        {
            System_Sleep(false);

            Serial.println("Awakening");

            display.fillScreen(BLACK);
            display.setTextSize(1);
            display.setCursor(5, 25);
            display.print("Awakening");
            display.display();

            Sleep_OP.current_mode = Sleep_OP.mode::NOT_SLEEP;
            Button_Triggered_OP.Interrupt_Flag = false;
            Sleep_OP.cycletime_1 = millis() + AUTOMATICALLY_ENTER_LIGHT_SLEEP_TIME;
        }
        else
        {
            Serial.println("Enter light sleep");
            gpio_hold_en(SLEEP_WAKE_UP_INT);
            esp_sleep_enable_ext0_wakeup(SLEEP_WAKE_UP_INT, LOW);
            esp_light_sleep_start();
        }
    }
    else
    {
        if (Key_Scanning() == true)
        {
            display.fillScreen(BLACK);
            display.setTextSize(1);
            display.setCursor(5, 25);

            switch (Button_Triggered_OP.current_state)
            {
            case Button_Triggered_OP.gesture::SINGLE_CLICK:
                Serial.println("Key triggered: SINGLE_CLICK");

                display.print("1.SINGLE_CLICK");
                display.display();

                Sleep_OP.cycletime_1 = millis() + AUTOMATICALLY_ENTER_LIGHT_SLEEP_TIME;

                break;
            case Button_Triggered_OP.gesture::DOUBLE_CLICK:
                Serial.println("Key triggered: DOUBLE_CLICK");

                display.print("2.DOUBLE_CLICK");
                display.display();

                Sleep_OP.cycletime_1 = millis() + AUTOMATICALLY_ENTER_LIGHT_SLEEP_TIME;

                break;
            case Button_Triggered_OP.gesture::LONG_PRESS:
                Serial.println("Key triggered: LONG_PRESS");

                display.print("3.LONG_PRESS");
                display.setCursor(5, 60);
                display.print("Deep Sleep");
                display.display();

                Sleep_OP.current_mode = Sleep_OP.mode::DEEP_SLEEP;

                delay(1000);

                System_Sleep(true);

                Serial.println("Enter deep sleep");
                // gpio_hold_en(SLEEP_WAKE_UP_INT);
                // esp_sleep_enable_ext0_wakeup(SLEEP_WAKE_UP_INT, LOW);
                esp_deep_sleep_start();

                while (1)
                {
                }
                // delay(1000);
                break;

            default:
                break;
            }
        }
    }
}
