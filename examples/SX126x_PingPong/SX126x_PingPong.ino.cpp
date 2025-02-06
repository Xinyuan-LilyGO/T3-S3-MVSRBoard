# 1 "C:\\Users\\XK\\AppData\\Local\\Temp\\tmpa3wdcaza"
#include <Arduino.h>
# 1 "D:/Information/visual_studio_code/GitHub/T3-S3-MVSRBoard/examples/SX126x_PingPong/SX126x_PingPong.ino"
# 12 "D:/Information/visual_studio_code/GitHub/T3-S3-MVSRBoard/examples/SX126x_PingPong/SX126x_PingPong.ino"
#include "RadioLib.h"
#include "pin_config.h"



#define INITIATING_NODE 






SX1262 radio = new Module(LORA_CS, LORA_DIO1, LORA_RST, LORA_BUSY, SPI);
# 34 "D:/Information/visual_studio_code/GitHub/T3-S3-MVSRBoard/examples/SX126x_PingPong/SX126x_PingPong.ino"
int transmissionState = RADIOLIB_ERR_NONE;


bool transmitFlag = false;


volatile bool operationDone = false;





#if defined(ESP8266) || defined(ESP32)
ICACHE_RAM_ATTR
#endif
void setFlag(void);
void setup();
void loop();
#line 49 "D:/Information/visual_studio_code/GitHub/T3-S3-MVSRBoard/examples/SX126x_PingPong/SX126x_PingPong.ino"
void setFlag(void)
{

    operationDone = true;
}

void setup()
{
    Serial.begin(115200);


    Serial.println("[SX1262] Initializing ... ");
    SPI.begin(LORA_SCLK, LORA_MISO, LORA_MOSI);

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

    radio.setFrequency(868.6);
    radio.setBandwidth(500.0);
    radio.setSpreadingFactor(9);
    radio.setCodingRate(6);
    radio.setSyncWord(0xAB);
    radio.setOutputPower(22);
    radio.setCurrentLimit(140);
    radio.setPreambleLength(16);
    radio.setCRC(false);



    radio.setDio1Action(setFlag);

#if defined(INITIATING_NODE)

    Serial.print("[SX1262] Sending first packet ... ");
    transmissionState = radio.startTransmit("Hello World!");
    transmitFlag = true;
#else

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
}

void loop()
{

    if (operationDone)
    {

        operationDone = false;

        if (transmitFlag)
        {


            if (transmissionState == RADIOLIB_ERR_NONE)
            {

                Serial.println("transmission finished!");
            }
            else
            {
                Serial.print("failed, code ");
                Serial.println(transmissionState);
            }


            radio.startReceive();
            transmitFlag = false;
        }
        else
        {


            String str;
            int state = radio.readData(str);

            if (state == RADIOLIB_ERR_NONE)
            {

                Serial.println("[SX1262] Received packet!");


                Serial.print("[SX1262] Data:\t\t");
                Serial.println(str);


                Serial.print("[SX1262] RSSI:\t\t");
                Serial.print(radio.getRSSI());
                Serial.println(" dBm");


                Serial.print("[SX1262] SNR:\t\t");
                Serial.print(radio.getSNR());
                Serial.println(" dB");
            }


            delay(1000);


            Serial.println("[SX1262] Sending another packet ... ");
            transmissionState = radio.startTransmit("Hello World!");
            transmitFlag = true;
        }
    }
}