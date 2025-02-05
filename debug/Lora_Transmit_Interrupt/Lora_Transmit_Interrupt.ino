/*
   RadioLib Transmit with Interrupts Example

   This example transmits packets using SX1276/SX1278/SX1262/SX1268/SX1280/LR1121 LoRa radio module.
   Each packet contains up to 256 bytes of data, in the form of:
    - Arduino String
    - null-terminated char array (C-string)
    - arbitrary binary data (byte array)

   For full API reference, see the GitHub Pages
   https://jgromes.github.io/RadioLib/
*/

#include <RadioLib.h>
#include "pin_config.h"

static const uint64_t Local_MAC = ESP.getEfuseMac();

uint8_t Send_Package[16] = {'M', 'A', 'C', ':',
                            (uint8_t)(Local_MAC >> 56), (uint8_t)(Local_MAC >> 48),
                            (uint8_t)(Local_MAC >> 40), (uint8_t)(Local_MAC >> 32),
                            (uint8_t)(Local_MAC >> 24), (uint8_t)(Local_MAC >> 16),
                            (uint8_t)(Local_MAC >> 8), (uint8_t)Local_MAC,
                            0, 0, 0, 0};

#ifdef T3_S3_SX1276
SX1276 radio = new Module(LORA_CS, LORA_DIO0, LORA_RST, LORA_DIO1, SPI);
#endif

#ifdef T3_S3_SX1278
SX1278 radio = new Module(LORA_CS, LORA_DIO0, LORA_RST, LORA_DIO1, SPI);
#endif

// save transmission state between loops
static int transmissionState = RADIOLIB_ERR_NONE;
// flag to indicate that a packet was sent
volatile bool transmittedFlag = false;

void setFlag(void)
{
    // we sent a packet, set the flag
    transmittedFlag = true;
}

void setup()
{
    Serial.begin(115200);

    SPI.begin(LORA_SCLK, LORA_MISO, LORA_MOSI);

    // initialize radio with default settings
    int state = radio.begin();

    Serial.print(F("Radio Initializing ... "));
    if (state == RADIOLIB_ERR_NONE)
    {
        Serial.println(F("success!"));
    }
    else
    {
        Serial.print(F("failed, code "));
        Serial.println(state);
        while (true)
            ;
    }

    // set the function that will be called
    // when packet transmission is finished
    radio.setPacketSentAction(setFlag);

    // radio.setFrequency(914.9);
    radio.setFrequency(868.0);
    radio.setBandwidth(125.0);
    // radio.setBitRate(300.0);
    radio.setSpreadingFactor(12);
    radio.setCodingRate(6);
    radio.setSyncWord(0xAB);
    radio.setCurrentLimit(240);
    radio.setOutputPower(17);
    radio.setPreambleLength(16);
    radio.setCRC(false);

    // start transmitting the first packet
    Serial.print(F("Radio Sending first packet ... "));

    radio.startTransmit(Send_Package, 16);
}

void loop()
{
    if (transmittedFlag)
    {
        transmittedFlag = false;


        Serial.println("Radio Sending another packet ... ");

        radio.startTransmit(Send_Package, 16);
    }
}