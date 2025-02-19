/*
 * @Description: None
 * @Author: LILYGO_L
 * @Date: 2024-07-25 16:34:24
 * @LastEditTime: 2024-07-25 17:26:25
 * @License: GPL 3.0
 */
#include <RadioLib.h>
#include "pin_config.h"

SX1262 radio = new Module(LORA_CS, LORA_DIO1, LORA_RST, LORA_BUSY, SPI);

// save transmission state between loops
int transmissionState = RADIOLIB_ERR_NONE;

volatile bool transmittedFlag = false;

void setFlag(void)
{
    // we sent a packet, set the flag
    transmittedFlag = true;
}

// counter to keep track of transmitted packets
int count = 0;

void setup()
{
    Serial.begin(115200);

    // initialize SX1262 with default settings
    SPI.begin(LORA_SCLK, LORA_MISO, LORA_MOSI);
    Serial.print("[SX1262] Initializing ... ");
    // int state = radio.beginFSK();
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

    radio.setFrequency(872.1);
    radio.setBandwidth(500);
    radio.setSpreadingFactor(9);
    radio.setCodingRate(6);
    radio.setSyncWord(0xAB);
    radio.setOutputPower(22);
    radio.setPreambleLength(16);
    radio.setCRC(false);

    // set the function that will be called
    // when packet transmission is finished
    radio.setPacketSentAction(setFlag);

    // start transmitting the first packet
    Serial.print("[SX1262] Sending first packet ... ");

    // you can transmit C-string or Arduino string up to
    // 256 characters long
    transmissionState = radio.startTransmit("Hello World!");
}

void loop()
{
    // check if the previous transmission finished
    if (transmittedFlag)
    {
        // reset flag
        transmittedFlag = false;

        if (transmissionState == RADIOLIB_ERR_NONE)
        {
            // packet was successfully sent
            Serial.println("transmission finished!");

            // NOTE: when using interrupt-driven transmit method,
            //       it is not possible to automatically measure
            //       transmission data rate using getDataRate()
        }
        else
        {
            Serial.print("failed, code ");
            Serial.println(transmissionState);
        }

        // clean up after transmission is finished
        // this will ensure transmitter is disabled,
        // RF switch is powered down etc.
        radio.finishTransmit();

        // wait a second before transmitting again
        delay(1000);

        // send another one
        Serial.print("[SX1262] Sending another packet ... ");

        // you can transmit C-string or Arduino string up to
        // 256 characters long
        String str = "Hello World! #" + String(count++);
        transmissionState = radio.startTransmit(str);
    }
}
