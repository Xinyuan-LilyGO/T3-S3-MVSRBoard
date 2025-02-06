/*
 * @Description:SD card test procedure.
 * @Author: LILYGO_L
 * @Date: 2023-08-18 15:33:23
 * @LastEditTime: 2025-02-05 17:05:19
 * @License: GPL 3.0
 */
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "pin_config.h"
#include "Audio.h"

#define SD_FILE_NAME "/music.mp3"

size_t CycleTime = 0;

bool Volume_Switching = 0;

File testFile;
Audio audio;

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");

    pinMode(0, INPUT_PULLUP);
    pinMode(MAX98357A_SD_MODE, OUTPUT);
    digitalWrite(MAX98357A_SD_MODE, HIGH);

    SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS); // SPI boots

    while (!SD.begin(SD_CS, SPI, 40000000)) // SD boots
    {

        Serial.println("Detecting SD card");

        Serial.println(".");
        delay(100);

        Serial.println(".");
        delay(100);

        Serial.println(".");
        delay(100);

        Serial.println(".");
        delay(100);

        Serial.println(".");
        delay(100);

        Serial.println("SD card initialization failed !");
        delay(100);

        SD.end();
    }

    Serial.println("SD card initialization successful !");
    delay(100);

    audio.setPinout(MAX98357A_BCLK, MAX98357A_LRCLK, MAX98357A_DATA);
    audio.setVolume(3); // 0...21,Volume setting
    // audio.setBalance(-16);

    audio.connecttoSD(SD_FILE_NAME);

    Serial.print("Start playing SD card music");
}

void loop()
{
    audio.loop();

    if (digitalRead(0) == LOW)
    {
        if (millis() > CycleTime)
        {
            Volume_Switching = !Volume_Switching;

            if (Volume_Switching == 1)
            {
                audio.setVolume(21);
            }
            else
            {
                audio.setVolume(3);
            }

            CycleTime = millis() + 300;
        }
    }
}

// optional
void audio_info(const char *info)
{
    Serial.print("info        ");
    Serial.println(info);
}
void audio_id3data(const char *info)
{ // id3 metadata
    Serial.print("id3data     ");
    Serial.println(info);
}
void audio_eof_mp3(const char *info)
{ // end of file
    Serial.print("eof_mp3     ");
    Serial.println(info);
}
void audio_showstation(const char *info)
{
    Serial.print("station     ");
    Serial.println(info);
}
void audio_showstreamtitle(const char *info)
{
    Serial.print("streamtitle ");
    Serial.println(info);
}
void audio_bitrate(const char *info)
{
    Serial.print("bitrate     ");
    Serial.println(info);
}
void audio_commercial(const char *info)
{ // duration in sec
    Serial.print("commercial  ");
    Serial.println(info);
}
void audio_icyurl(const char *info)
{ // homepage
    Serial.print("icyurl      ");
    Serial.println(info);
}
void audio_lasthost(const char *info)
{ // stream URL played
    Serial.print("lasthost    ");
    Serial.println(info);
}