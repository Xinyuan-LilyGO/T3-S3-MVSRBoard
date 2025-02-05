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

#ifdef T3_S3_SX1276
SX1276 radio = new Module(LORA_CS, LORA_DIO0, LORA_RST, LORA_DIO1, SPI);
#endif

#ifdef T3_S3_SX1278
SX1278 radio = new Module(LORA_CS, LORA_DIO0, LORA_RST, LORA_DIO1, SPI);
#endif

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

volatile bool Lora_Receive_Flag = false;

void Lora_Receive_Flag_Callback(void)
{
    Lora_Receive_Flag = true;
}

void setup()
{
    Serial.begin(115200);

    // initialize SX1276 with default settings
    Serial.println("[SX1276] Initializing ... ");
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
    radio.setFrequency(868.1);
    radio.setBandwidth(500.0);
    // radio.setBitRate(300.0);
    radio.setSpreadingFactor(9);
    radio.setCodingRate(6);
    radio.setSyncWord(0xAB);
    radio.setCurrentLimit(240);
    radio.setOutputPower(17);
    radio.setPreambleLength(16);
    radio.setCRC(false);

    radio.setPacketReceivedAction(Lora_Receive_Flag_Callback);

    radio.startReceive();
}

void loop()
{
    if (digitalRead(0) == LOW)
    {
        delay(300);
        Lora_Mode = !Lora_Mode;

        radio.begin();
        // radio.setFrequency(914.9);
        radio.setFrequency(868.1);
        radio.setBandwidth(500.0);
        // radio.setBitRate(300.0);
        radio.setSpreadingFactor(9);
        radio.setCodingRate(6);
        radio.setSyncWord(0xAB);
        radio.setCurrentLimit(240);
        radio.setOutputPower(17);
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
            Serial.println("[SX1276] Sending another packet ... ");

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
        if (Lora_Receive_Flag)
        {
            Lora_Receive_Flag = false;

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
                        Serial.println("[SX1276] Received packet!");

                        // print data of the packet
                        for (int i = 0; i < 16; i++)
                        {
                            Serial.printf("[SX1276] Data[%d]: %#X\n", i, Receive_Package[i]);
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
                        Serial.print("[SX1276] Data:\t\t");
                        Serial.println(Receive_Data);

                        // print RSSI (Received Signal Strength Indicator)
                        Serial.print("[SX1276] RSSI:\t\t");
                        Serial.print(radio.getRSSI());
                        Serial.println(" dBm");

                        // print SNR (Signal-to-Noise Ratio)
                        Serial.print("[SX1276] SNR:\t\t");
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
