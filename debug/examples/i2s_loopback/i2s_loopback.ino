#include <Arduino.h>
#include <driver/i2s.h>
#include <codec2.h>
#include "pin_config.h"

// serial
#define SERIAL_BAUD_RATE 115200

// audio speaker
#define AUDIO_SPEAKER_BCLK MAX98357A_BCLK
#define AUDIO_SPEAKER_LRC MAX98357A_LRCLK
#define AUDIO_SPEAKER_DIN MAX98357A_DATA

// audio microphone
#define AUDIO_MIC_SD MSM261_DATA
#define AUDIO_MIC_WS MSM261_WS
#define AUDIO_MIC_SCK MSM261_BCLK

#define AUDIO_SAMPLE_RATE 8000 // 44100

TaskHandle_t audio_task_;
struct CODEC2 *c2_;
int c2_samples_per_frame_;
int c2_bytes_per_frame_;
int16_t *c2_samples_;
uint8_t *c2_bits_;

void setup()
{
    Serial.begin(SERIAL_BAUD_RATE);
    Serial.println("Ciallo");

    pinMode(MSM261_EN, OUTPUT);
    digitalWrite(MSM261_EN, HIGH);

    pinMode(MAX98357A_SD_MODE, OUTPUT);
    digitalWrite(MAX98357A_SD_MODE, HIGH);

    Serial.println("Board setup started");

    // create i2s speaker
    i2s_config_t i2s_speaker_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = AUDIO_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 1024,
        .use_apll = 0,
        .tx_desc_auto_clear = true,
        .fixed_mclk = -1};
    i2s_pin_config_t i2s_speaker_pin_config = {
        .bck_io_num = AUDIO_SPEAKER_BCLK,
        .ws_io_num = AUDIO_SPEAKER_LRC,
        .data_out_num = AUDIO_SPEAKER_DIN,
        .data_in_num = I2S_PIN_NO_CHANGE};
    if (i2s_driver_install(I2S_NUM_0, &i2s_speaker_config, 0, NULL) != ESP_OK)
    {
        Serial.println("Failed to install i2s speaker driver");
    }
    if (i2s_set_pin(I2S_NUM_0, &i2s_speaker_pin_config) != ESP_OK)
    {
        Serial.println("Failed to set i2s speaker pins");
    }

    // create i2s microphone
    i2s_config_t i2s_mic_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = AUDIO_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
        .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 1024,
        .use_apll = 0,
        .tx_desc_auto_clear = true,
        .fixed_mclk = -1};
    i2s_pin_config_t i2s_mic_pin_config = {
        .bck_io_num = AUDIO_MIC_SCK,
        .ws_io_num = AUDIO_MIC_WS,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = AUDIO_MIC_SD};
    if (i2s_driver_install(I2S_NUM_1, &i2s_mic_config, 0, NULL) != ESP_OK)
    {
        Serial.println("Failed to install i2s mic driver");
    }
    if (i2s_set_pin(I2S_NUM_1, &i2s_mic_pin_config) != ESP_OK)
    {
        Serial.println("Failed to set i2s mic pins");
    }

    // run codec2 audio loopback on a separate task
    xTaskCreate(&audio_task, "audio_task", 32000, NULL, 5, &audio_task_);

    Serial.println("Board setup completed");
}

void audio_task(void *param)
{
    // construct and configure codec2
    c2_ = codec2_create(CODEC2_MODE_3200);
    if (c2_ == NULL)
    {
        Serial.println("Failed to create Codec2");
        return;
    }
    else
    {
        // codec2_set_lpc_post_filter(c2_, 1, 0, 0.8, 0.2);
        c2_samples_per_frame_ = codec2_samples_per_frame(c2_);
        c2_bytes_per_frame_ = codec2_bytes_per_frame(c2_);
        c2_samples_ = (int16_t *)malloc(sizeof(int16_t) * c2_samples_per_frame_);
        c2_bits_ = (uint8_t *)malloc(sizeof(uint8_t) * c2_bytes_per_frame_);
        // Serial.println("Codec2 constructed", c2_samples_per_frame_, c2_bytes_per_frame_);
    }

    // run loopback record-encode-decode-playback loop
    size_t bytes_read, bytes_written;
    Serial.println("Audio task started");
    while (true)
    {
        i2s_read(I2S_NUM_1, c2_samples_, sizeof(uint16_t) * c2_samples_per_frame_, &bytes_read, portMAX_DELAY);
        uint32_t startTimeEncode = millis();
        codec2_encode(c2_, c2_bits_, c2_samples_);
        Serial.println("Done encoding, took ms: " + String(millis() - startTimeEncode));
        vTaskDelay(1);
        uint32_t startTimeDecode = millis();
        codec2_decode(c2_, c2_samples_, c2_bits_);
        Serial.println("Done encoding, took ms: " + String(millis() - startTimeEncode));
        i2s_write(I2S_NUM_0, c2_samples_, sizeof(uint16_t) * c2_samples_per_frame_, &bytes_written, portMAX_DELAY);
        vTaskDelay(1);
    }
}

void loop()
{
    // do nothing
    delay(100);
}
