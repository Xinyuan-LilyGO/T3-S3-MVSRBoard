/*
   RadioLib SX126x Ping-Pong Example

   For default module settings, see the wiki page
   https://github.com/jgromes/RadioLib/wiki/Default-configuration#sx126x---lora-modem

   For full API reference, see the GitHub Pages
   https://jgromes.github.io/RadioLib/
*/

// include the library
#include "RadioLib.h"
#include "pin_config.h"

// uncomment the following only on one
// of the nodes to initiate the pings
// #define INITIATING_NODE

static const uint64_t Local_MAC = ESP.getEfuseMac();

static size_t CycleTime = 0;

// SX1262 has the following connections:
// NSS pin:   10
// DIO1 pin:  2
// NRST pin:  3
// BUSY pin:  9
SX1262 radio = new Module(LORA_CS, LORA_DIO1, LORA_RST, LORA_BUSY, SPI);

// or using RadioShield
// https://github.com/jgromes/RadioShield
// SX1262 radio = RadioShield.ModuleA;

// or using CubeCell
// SX1262 radio = new Module(RADIOLIB_BUILTIN_MODULE);

uint8_t Receive_Package[16];
uint32_t Receive_Data = 0;

uint8_t Send_Package[16] = {'M', 'A', 'C', ':',
                            (uint8_t)(Local_MAC >> 56), (uint8_t)(Local_MAC >> 48),
                            (uint8_t)(Local_MAC >> 40), (uint8_t)(Local_MAC >> 32),
                            (uint8_t)(Local_MAC >> 24), (uint8_t)(Local_MAC >> 16),
                            (uint8_t)(Local_MAC >> 8), (uint8_t)Local_MAC,
                            0, 0, 0, 0};

uint32_t Send_Data = 0;
bool Send_Flag = false;

// flag to indicate that a packet was sent or received
volatile bool operationDone = false;

void setFlag(void)
{
    // we sent or received a packet, set the flag
    operationDone = true;
}

void setup()
{
    Serial.begin(115200);

    // initialize SX1262 with default settings
    Serial.println("[SX1262] Initializing ... ");
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

    // radio.setFrequency(914.9);
    radio.setFrequency(914.8);
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

    Send_Flag = true;
    CycleTime = millis() + 1000;

    // // start listening for LoRa packets on this node
    // Serial.println("[SX1262] Starting to listen ... ");
    // state = radio.startReceive();
    // if (state == RADIOLIB_ERR_NONE)
    // {
    //     Serial.println("success!");
    // }
    // else
    // {
    //     Serial.print("failed, code ");
    //     Serial.println(state);
    //     while (true)
    //         ;
    // }
}

void loop()
{
    if (Send_Flag == true)
    {
        if (millis() > CycleTime)
        {
            Send_Flag = false;
            // send another one
            Serial.println("[SX1262] Sending another packet ... ");

            Send_Package[12] = (uint8_t)(Send_Data >> 24);
            Send_Package[13] = (uint8_t)(Send_Data >> 16);
            Send_Package[14] = (uint8_t)(Send_Data >> 8);
            Send_Package[15] = (uint8_t)Send_Data;

            radio.transmit(Send_Package, 16);
            radio.startReceive();
        }
    }

    // check if the previous operation finished
    if (operationDone)
    {
        // reset flag
        operationDone = false;

        if (radio.readData(Receive_Package, 16) == RADIOLIB_ERR_NONE)
        {
            if ((Receive_Package[0] == 'M') &&
                (Receive_Package[1] == 'A') &&
                (Receive_Package[2] == 'C') &&
                (Receive_Package[3] == ':'))
            {
                uint64_t temp_mac =
                    ((uint64_t)Receive_Package[4] << 56) |
                    ((uint64_t)Receive_Package[5] << 48) |
                    ((uint64_t)Receive_Package[6] << 40) |
                    ((uint64_t)Receive_Package[7] << 32) |
                    ((uint64_t)Receive_Package[8] << 24) |
                    ((uint64_t)Receive_Package[9] << 16) |
                    ((uint64_t)Receive_Package[10] << 8) |
                    (uint64_t)Receive_Package[11];

                if (temp_mac != Local_MAC)
                {
                    Receive_Data =
                        ((uint32_t)Receive_Package[12] << 24) |
                        ((uint32_t)Receive_Package[13] << 16) |
                        ((uint32_t)Receive_Package[14] << 8) |
                        (uint32_t)Receive_Package[15];

                    // packet was successfully received
                    Serial.println("[SX1262] Received packet!");

                    // print data of the packet
                    for (int i = 0; i < 16; i++)
                    {
                        Serial.printf("[SX1262] Data[%d]: %#X\n", i, Receive_Package[i]);
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
                    Serial.print("[SX1262] Data:\t\t");
                    Serial.println(Receive_Data);

                    // print RSSI (Received Signal Strength Indicator)
                    Serial.print("[SX1262] RSSI:\t\t");
                    Serial.print(radio.getRSSI());
                    Serial.println(" dBm");

                    // print SNR (Signal-to-Noise Ratio)
                    Serial.print("[SX1262] SNR:\t\t");
                    Serial.print(radio.getSNR());
                    Serial.println(" dB");

                    Send_Data = Receive_Data + 1;

                    Send_Flag = true;
                    CycleTime = millis() + 1000;
                }
            }
        }
    }
}
