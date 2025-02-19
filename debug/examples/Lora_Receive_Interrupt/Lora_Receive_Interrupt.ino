/*
   RadioLib Receive with Interrupts Example

   This example listens for LoRa transmissions and tries to
   receive them. Once a packet is received, an interrupt is
   triggered. To successfully receive data, the following
   settings have to be the same on both transmitter
   and receiver:
    - carrier frequency
    - bandwidth
    - spreading factor
    - coding rate
    - sync word

   For full API reference, see the GitHub Pages
   https://jgromes.github.io/RadioLib/
*/

#include <RadioLib.h>
#include "pin_config.h"

uint8_t Receive_Package[16];

#ifdef T3_S3_SX1276
SX1276 radio = new Module(LORA_CS, LORA_DIO0, LORA_RST, LORA_DIO1, SPI);
#endif

#ifdef T3_S3_SX1278
SX1278 radio = new Module(LORA_CS, LORA_DIO0, LORA_RST, LORA_DIO1, SPI);
#endif

// flag to indicate that a packet was received
volatile bool receivedFlag = false;

void setFlag(void)
{
    // we got a packet, set the flag
    receivedFlag = true;
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
    // when new packet is received
    radio.setPacketReceivedAction(setFlag);
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

    // start listening for LoRa packets
    Serial.print(F("Radio Starting to listen ... "));
    state = radio.startReceive();
    if (state == RADIOLIB_ERR_NONE)
    {
        Serial.println(F("success!"));
    }
    else
    {
        Serial.print(F("failed, code "));
        Serial.println(state);
    }
}

void loop()
{
    // check if the flag is set
    if (receivedFlag)
    {
        // reset flag
        receivedFlag = false;

        if (radio.readData(Receive_Package, 16) == RADIOLIB_ERR_NONE)
        {
            // packet was successfully received
            Serial.println(F("Radio Received packet!"));

            // print RSSI (Received Signal Strength Indicator)
            Serial.print("[SX1276] RSSI:\t\t");
            Serial.print(radio.getRSSI());
            Serial.println(" dBm");

            // print SNR (Signal-to-Noise Ratio)
            Serial.print("[SX1276] SNR:\t\t");
            Serial.print(radio.getSNR());
            Serial.println(" dB");
        }

        // put module back to listen mode
        radio.startReceive();
    }
}