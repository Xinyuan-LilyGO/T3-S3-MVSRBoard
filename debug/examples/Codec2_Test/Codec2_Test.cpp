#include <Arduino.h>
#include <codec2.h>

struct CODEC2 *codec2_state;

int16_t sine1KHz[8] = {-21210, -30000, -21210, 0, 21210, 30000, 21210, 0};

int16_t audioBuf[320];
uint8_t c2Buf[8];
boolean flagEncodeAudioBuf = false;
boolean flagDecodeC2Buf = false;

void c2_encode()
{
    flagEncodeAudioBuf = true;
    while (flagEncodeAudioBuf)
        delay(1); // Wait for the codec2_watch
}

void c2_decode()
{
    flagDecodeC2Buf = true;
    while (flagDecodeC2Buf)
        delay(1); // Wait for the codec2_watch
}

void codec2_watcher(void *parameter)
{
    while (true)
    {
        // yield() DOES NOT work, that trips the WDT every 5 secs
        // delay(1) is VITAL
        delay(1);
        if (flagEncodeAudioBuf)
        { // We have some work to do
            codec2_encode(codec2_state, c2Buf, audioBuf);
            flagEncodeAudioBuf = false; // Notify encode()
        }
        if (flagDecodeC2Buf)
        { // We have some work to do
            codec2_decode(codec2_state, audioBuf, c2Buf);
            flagDecodeC2Buf = false; // Notify decode()
        }
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println();
    Serial.println("Started codec2 sample");

    // Set bitrate
    codec2_state = codec2_create(CODEC2_MODE_1600);
    // Set some tuning parameters
    codec2_set_lpc_post_filter(codec2_state, 1, 0, 0.8, 0.2);

    xTaskCreate(&codec2_watcher, "codec2_watcher_task", 30000, NULL, 5, NULL);

    // Encode a (fixed size) input buffer of type int16_t to a (fixed size) output buffer
    // CODEC2_MODE_1600 encodes 320 speech samples (320 * 2 = 640 bytes / 40ms of speech) into 8 bytes (64 bits)
    // Fill buffer with a sine wave
    for (int i = 0; i < 320; i++)
        audioBuf[i] = sine1KHz[i % 8];

    for (int i = 0; i < 320; i++)
        Serial.print(String(audioBuf[i], DEC) + " ");
    Serial.println();

    int startTimeEncode = millis();
    c2_encode();
    Serial.println("Done encoding, took ms: " + String(millis() - startTimeEncode));

    for (int i = 0; i < 8; i++)
        Serial.print(String(c2Buf[i], DEC) + " ");
    Serial.println();

    int startTimeDecode = millis();
    c2_decode();
    Serial.println("Done decoding, took ms: " + String(millis() - startTimeDecode));

    for (int i = 0; i < 320; i++)
        Serial.print(String(audioBuf[i], DEC) + " ");
    Serial.println();
}

void loop()
{
}
