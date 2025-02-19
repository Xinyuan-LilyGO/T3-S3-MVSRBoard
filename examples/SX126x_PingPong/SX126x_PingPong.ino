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
#define INITIATING_NODE

uint8_t Send_Package[9] = {9, 8, 7, 6, 5, 4, 3, 2, 1};

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

// save transmission states between loops
int transmissionState = RADIOLIB_ERR_NONE;

// flag to indicate transmission or reception state
bool transmitFlag = false;

// flag to indicate that a packet was sent or received
volatile bool operationDone = false;

// this function is called when a complete packet
// is transmitted or received by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
#if defined(ESP8266) || defined(ESP32)
ICACHE_RAM_ATTR
#endif
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

    // radio.setFrequency(868.1);
    // radio.setBandwidth(125.0);
    // radio.setSpreadingFactor(9);
    // radio.setCodingRate(7);
    // radio.setSyncWord(RADIOLIB_SX126X_SYNC_WORD_PRIVATE);
    // radio.setOutputPower(22);
    // radio.setCurrentLimit(140);
    // radio.setPreambleLength(8);
    // radio.setCRC(true);

    radio.setFrequency(868.1);
    // radio.setFrequencyDeviation(5.0);
    radio.setBitRate(300.0);
    radio.setRxBandwidth(467.0);
    radio.setSpreadingFactor(9);
    radio.setOutputPower(22);
    radio.setCurrentLimit(140);
    radio.setPreambleLength(16);
    radio.setCRC(0, 0x1D0F, 0x1021, true);
    // uint8_t syncWord[] = {0x12, 0xAD};
    // state = radio.setSyncWord(syncWord, 2);

    // set the function that will be called
    // when new packet is received
    radio.setDio1Action(setFlag);

#if defined(INITIATING_NODE)
    // send the first packet on this node
    Serial.print("[SX1262] Sending first packet ... ");
    transmissionState = radio.transmit(Send_Package, 9);
    transmitFlag = true;
#else
    // start listening for LoRa packets on this node
    Serial.println("[SX1262] Starting to listen ... ");
    state = radio.startReceive();
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
#endif

    Serial.printf("spreadingFactor: %d\n", radio.spreadingFactor);
    Serial.printf("codingRate: %d\n", radio.codingRate);
    Serial.printf("ldrOptimize: %d\n", radio.ldrOptimize);
    Serial.printf("crcTypeLoRa: %d\n", radio.crcTypeLoRa);
    Serial.printf("headerType: %d\n", radio.headerType);
    Serial.printf("preambleLengthLoRa: %d\n", radio.preambleLengthLoRa);
    Serial.printf("bandwidthKhz: %.2f\n", radio.bandwidthKhz);
    Serial.printf("ldroAuto: %s\n", radio.ldroAuto ? "true" : "false");
    Serial.printf("bitRate: %lu\n", radio.bitRate);
    Serial.printf("frequencyDev: %lu\n", radio.frequencyDev);
    Serial.printf("rxBandwidth: %d\n", radio.rxBandwidth);
    Serial.printf("pulseShape: %d\n", radio.pulseShape);
    Serial.printf("crcTypeFSK: %d\n", radio.crcTypeFSK);
    Serial.printf("syncWordLength: %d\n", radio.syncWordLength);
    Serial.printf("addrComp: %d\n", radio.addrComp);
    Serial.printf("whitening: %d\n", radio.whitening);
    Serial.printf("packetType: %d\n", radio.packetType);
    Serial.printf("preambleLengthFSK: %d\n", radio.preambleLengthFSK);
    Serial.printf("rxBandwidthKhz: %.2f\n", radio.rxBandwidthKhz);
    Serial.printf("dataRateMeasured: %.2f\n", radio.dataRateMeasured);
    Serial.printf("tcxoDelay: %lu\n", radio.tcxoDelay);
    Serial.printf("pwr: %d\n", radio.pwr);
    Serial.printf("implicitLen: %zu\n", radio.implicitLen);
    Serial.printf("invertIQEnabled: %d\n", radio.invertIQEnabled);
}

void loop()
{
    // check if the previous operation finished
    if (operationDone)
    {
        // reset flag
        operationDone = false;

        if (transmitFlag)
        {
            // the previous operation was transmission, listen for response
            // print the result
            if (transmissionState == RADIOLIB_ERR_NONE)
            {
                // packet was successfully sent
                Serial.println("transmission finished!");
            }
            else
            {
                Serial.print("failed, code ");
                Serial.println(transmissionState);
            }

            // listen for response
            radio.startReceive();
            transmitFlag = false;
        }
        else
        {
            // the previous operation was reception
            // print data and send another packet
            uint8_t receive_data[255] = {0};
            int state = radio.readData(receive_data, 9);

            if (state == RADIOLIB_ERR_NONE)
            {
                // packet was successfully received
                Serial.println("[SX1262] Received packet!");

                // print data of the packet
                for (uint8_t i = 0; i < 9; i++)
                {
                    printf("[SX1262] data[%d]: %d\n", i, receive_data[i]);
                }

                // print RSSI (Received Signal Strength Indicator)
                Serial.print("[SX1262] RSSI:\t\t");
                Serial.print(radio.getRSSI());
                Serial.println(" dBm");

                // print SNR (Signal-to-Noise Ratio)
                Serial.print("[SX1262] SNR:\t\t");
                Serial.print(radio.getSNR());
                Serial.println(" dB");
            }

            // wait a second before transmitting again
            delay(1000);

            // send another one
            // Serial.println("[SX1262] Sending another packet ... ");
            // transmissionState = radio.startTransmit(Send_Package, 9);
            // transmitFlag = true;
        }
    }
}
