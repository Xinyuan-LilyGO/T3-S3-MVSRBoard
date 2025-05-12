/*
 * @Description: SX128x_PingPong_2 test
 * @Author: LILYGO_L
 * @Date: 2024-12-02 10:43:22
 * @LastEditTime: 2025-05-08 14:25:21
 * @License: GPL 3.0
 */
#include "RadioLib.h"
#include "pin_config.h"

// uncomment the following only on one
// of the nodes to initiate the pings
// #define INITIATING_NODE

static const uint64_t Local_MAC = ESP.getEfuseMac();

static size_t CycleTime = 0;

// SX1280 has the following connections:
// NSS pin:   10
// DIO1 pin:  2
// NRST pin:  3
// BUSY pin:  9
#if defined(T3_S3_SX1280) || defined(T3_S3_SX1280PA)
SX1280 radio = new Module(LORA_CS, LORA_DIO1, LORA_RST, LORA_BUSY, SPI);
#endif

// or using RadioShield
// https://github.com/jgromes/RadioShield
// SX1280 radio = RadioShield.ModuleA;

// or using CubeCell
// SX1280 radio = new Module(RADIOLIB_BUILTIN_MODULE);

uint8_t Receive_Package[16];
uint32_t Receive_Data = 0;

uint8_t Send_Package[16] = {'M', 'A', 'C', ':',
                            (uint8_t)(Local_MAC >> 56), (uint8_t)(Local_MAC >> 48),
                            (uint8_t)(Local_MAC >> 40), (uint8_t)(Local_MAC >> 32),
                            (uint8_t)(Local_MAC >> 24), (uint8_t)(Local_MAC >> 16),
                            (uint8_t)(Local_MAC >> 8), (uint8_t)Local_MAC,
                            0, 0, 0, 0};

uint32_t Send_Data = 0;
bool Lora_Mode = 1;

volatile bool Lora_Transmission_Flag = false;

void Lora_Transmission_Flag_Callback(void)
{
    Lora_Transmission_Flag = true;
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");
    Serial.println("202505081425");

    // initialize SX1280 with default settings
    Serial.println("[SX1280] Initializing ... ");
    SPI.begin(LORA_SCLK, LORA_MISO, LORA_MOSI);
    int state = radio.begin();
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

#if defined(LORA_RX) && defined(LORA_TX)
    // The SX1280 version needs to set RX, TX antenna switching pins
    radio.setRfSwitchPins(LORA_RX, LORA_TX);
#endif

    // radio.setFrequency(914.9);
    radio.setFrequency(2400.1);
    radio.setBandwidth(1625.0);
    radio.setSpreadingFactor(12);
    radio.setCodingRate(6);
    radio.setSyncWord(0xAB);
    radio.setOutputPower(3);
    radio.setPreambleLength(16);
    radio.setCRC(false);

    radio.setPacketReceivedAction(Lora_Transmission_Flag_Callback);

    radio.startReceive();
}

void loop()
{
    if (digitalRead(0) == LOW)
    {
        delay(300);
        Lora_Mode = !Lora_Mode;

        radio.begin();
        radio.setFrequency(2400.1);
        radio.setBandwidth(1625.0);
        radio.setSpreadingFactor(12);
        radio.setCodingRate(6);
        radio.setSyncWord(0xAB);
        radio.setOutputPower(3);
        radio.setPreambleLength(16);
        radio.setCRC(false);

        if (Lora_Mode == 1)
        {
            radio.startReceive();
        }
    }

    if (Lora_Mode == 0)
    {
        if (millis() > CycleTime)
        {
            // send another one
            Serial.println("[SX1280] Sending another packet ... ");

            Send_Package[12] = (uint8_t)(Send_Data >> 24);
            Send_Package[13] = (uint8_t)(Send_Data >> 16);
            Send_Package[14] = (uint8_t)(Send_Data >> 8);
            Send_Package[15] = (uint8_t)Send_Data;

            radio.transmit(Send_Package, 16);

            CycleTime = millis() + 100;
        }
    }
    else
    {
        if (Lora_Transmission_Flag)
        {
            Lora_Transmission_Flag = false;

            if (radio.readData(Receive_Package, 16) == RADIOLIB_ERR_NONE)
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
                        Receive_Data =
                            ((uint32_t)Receive_Package[12] << 24) |
                            ((uint32_t)Receive_Package[13] << 16) |
                            ((uint32_t)Receive_Package[14] << 8) |
                            (uint32_t)Receive_Package[15];

                        // packet was successfully received
                        Serial.println("[SX1280] Received packet!");

                        // print data of the packet
                        for (int i = 0; i < 16; i++)
                        {
                            Serial.printf("[SX1280] Data[%d]: %#X\n", i, Receive_Package[i]);
                        }

                        // uint32_t temp_mac[2];
                        // temp_mac[0] =
                        //     ((uint32_t)Receive_Package[8] << 24) |
                        //     ((uint32_t)Receive_Package[9] << 16) |
                        //     ((uint32_t)Receive_Package[10] << 8) |
                        //     (uint32_t)Receive_Package[11];
                        // temp_mac[1] =
                        //     ((uint32_t)Receive_Package[4] << 24) |
                        //     ((uint32_t)Receive_Package[5] << 16) |
                        //     ((uint32_t)Receive_Package[6] << 8) |
                        //     (uint32_t)Receive_Package[7];

                        // Serial.printf("Chip Mac ID[0]: %u\n", temp_mac[0]);
                        // Serial.printf("Chip Mac ID[1]: %u\n", temp_mac[1]);

                        // print data of the packet
                        Serial.print("[SX1280] Data:\t\t");
                        Serial.println(Receive_Data);

                        // print RSSI (Received Signal Strength Indicator)
                        Serial.print("[SX1280] RSSI:\t\t");
                        Serial.print(radio.getRSSI());
                        Serial.println(" dBm");

                        // print SNR (Signal-to-Noise Ratio)
                        Serial.print("[SX1280] SNR:\t\t");
                        Serial.print(radio.getSNR());
                        Serial.println(" dB");

                        Send_Data = Receive_Data + 1;

                        radio.startReceive();
                    }
                }
            }
        }
    }
}
