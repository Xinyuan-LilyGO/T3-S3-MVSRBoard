/*
 * @Description(CN):
 *      播放Wifi音乐
 *
 * @Description(EN):
 *      Play WiFi music
 *
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-06-12 14:27:51
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2024-10-09 22:07:22
 * @License: GPL 3.0
 */
#include "Arduino.h"
#include "WiFi.h"
#include "Audio.h"
#include "SPI.h"
#include "SD.h"
#include "FS.h"
#include "pin_config.h"

// String ssid = "LilyGo-AABB";
// String password = "xinyuandianzi";
String ssid = "000000";
String password = "88888888";

Audio audio;

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");

    pinMode(MAX98357A_SD_MODE, OUTPUT);
    digitalWrite(MAX98357A_SD_MODE, HIGH);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    WiFi.setTxPower(WIFI_POWER_19_5dBm);

    WiFi.begin(ssid.c_str(), password.c_str());
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi connection failed");
        delay(1000);
    }

    Serial.println("WiFi connection successful");

    audio.setPinout(MAX98357A_BCLK, MAX98357A_LRCLK, MAX98357A_DATA);
    audio.setVolume(21); // 0...21,Volume setting
                         // audio.setBalance(-16);
    audio.setConnectionTimeout(2000, 7200);

    Serial.println("Trying to play music...");

    audio.connecttohost("http://music.163.com/song/media/outer/url?id=28457517.mp3");
}

void loop()
{
    audio.loop();
}

// optional
void audio_info(const char *info)
{
    Serial.print("info        ");
    Serial.println(info);
    // tft.print("info        ");
    // tft.println(info);
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
