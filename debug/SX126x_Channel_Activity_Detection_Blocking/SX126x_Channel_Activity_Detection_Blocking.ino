/*
 * @Description: None
 * @Author: None
 * @Date: 2024-07-23 14:07:02
 * @LastEditTime: 2024-07-23 14:11:02
 * @License: GPL 3.0
 */
/*
  RadioLib SX126x Blocking Channel Activity Detection Example

  This example uses SX1262 to scan the current LoRa
  channel and detect ongoing LoRa transmissions.
  Unlike SX127x CAD, SX126x can detect any part
  of LoRa transmission, not just the preamble.

  Other modules from SX126x family can also be used.

  Using blocking CAD is not recommended, as it will lead
  to significant amount of timeouts, inefficient use of processor
  time and can some miss packets!
  Instead, interrupt CAD is recommended.

  For default module settings, see the wiki page
  https://github.com/jgromes/RadioLib/wiki/Default-configuration#sx126x---lora-modem

  For full API reference, see the GitHub Pages
  https://jgromes.github.io/RadioLib/
*/

// include the library
#include "RadioLib.h"
#include "pin_config.h"

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

void setup()
{
    Serial.begin(115200);

    // initialize SX1262 with default settings
    Serial.print(F("[SX1262] Initializing ... "));
    SPI.begin(LORA_SCLK, LORA_MISO, LORA_MOSI);
    int state = radio.begin();
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
}

void loop()
{
    Serial.println("[SX1262] Scanning channel for LoRa transmission ... ");

    // start scanning current channel
    int state = radio.scanChannel();

    if (state == RADIOLIB_LORA_DETECTED)
    {
        // LoRa preamble was detected
        Serial.println("detected!");
    }
    else if (state == RADIOLIB_CHANNEL_FREE)
    {
        // no preamble was detected, channel is free
        Serial.println("channel is free!");
    }
    else
    {
        // some other error occurred
        Serial.print("failed, code ");
        Serial.println(state);
    }

    delay(100);
}
