/**
 * @file main.cpp
 * @brief Full TTS Visualization Demo for ESP32-P4 with GC9A01 Display
 * 
 * This example demonstrates complete TTS visualization including:
 * - Center phoneme display with IPA characters
 * - Top arc: IPA phonetic transcription with character-level tracking
 * - Bottom arc: Word display with character-level tracking
 * - Outer ring: A-Z alphabet with active letter highlighting & glow
 * - Radial spectrum analyzer (32 stationary radial bars with formant weighting)
 * - HUD Volume Overlay with animated circular progress gauge
 * - Idle / silence state handling with customizable arc text
 * 
 * Hardware Requirements:
 * - ESP32-P4 development board
 * - GC9A01 240x240 circular display (SPI)
 * 
 * This demo uses simulated TTS data. For real TTS, integrate with:
 * - esp32-p4-idf-formant-tts (formant synthesizer)
 * - esp-idf-espure-g2p (grapheme-to-phoneme)
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "formant_tts_display.h"

static const char* TAG = "tts_demo";

// ============================================================================
// Simulated TTS Data for Demo
// ============================================================================

struct PhonemeStep {
    const char* ipa_symbol;     // IPA symbol displayed in center (e.g. "h", "a", "l", "oː")
    int ipa_char_idx;           // UTF-8 char index in word's full IPA string for top arc
    int word_char_idx;          // Char index in word string for bottom arc
};

struct TTS_Example {
    const char* word;           // Normal orthography (bottom arc)
    const char* ipa;            // Full IPA phonetic transcription (top arc)
    int step_count;
    PhonemeStep steps[8];
};

// Example words with real IPA transcriptions and exact phoneme-to-character synchronization
static const TTS_Example examples[] = {
    {
        .word = "Hallo",
        .ipa = "haˈloː",
        .step_count = 4,
        .steps = {
            {"h",  0, 0},   // 'h' -> 'H'
            {"a",  1, 1},   // 'a' -> 'a'
            {"l",  3, 2},   // 'l' -> 'l'
            {"oː", 4, 4},   // 'o' -> 'o'
        },
    },
    {
        .word = "Guten",
        .ipa = "ˈɡʊtən",
        .step_count = 5,
        .steps = {
            {"ɡ",  1, 0},   // 'ɡ' -> 'G'
            {"ʊ",  2, 1},   // 'ʊ' -> 'u'
            {"t",  3, 2},   // 't' -> 't'
            {"ə",  4, 3},   // 'ə' -> 'e'
            {"n",  5, 4},   // 'n' -> 'n'
        },
    },
    {
        .word = "Tag",
        .ipa = "taːk",
        .step_count = 3,
        .steps = {
            {"t",  0, 0},   // 't' -> 'T'
            {"aː", 1, 1},   // 'a' -> 'a'
            {"k",  3, 2},   // 'k' -> 'g'
        },
    },
    {
        .word = "Testing",
        .ipa = "ˈtɛstɪŋ",
        .step_count = 6,
        .steps = {
            {"t",  1, 0},   // 't' -> 'T'
            {"ɛ",  2, 1},   // 'ɛ' -> 'e'
            {"s",  3, 2},   // 's' -> 's'
            {"t",  4, 3},   // 't' -> 't'
            {"ɪ",  5, 4},   // 'ɪ' -> 'i'
            {"ŋ",  6, 5},   // 'ŋ' -> 'n'
        },
    },
    {
        .word = "World",
        .ipa = "wɜːld",
        .step_count = 4,
        .steps = {
            {"w",  0, 0},   // 'w' -> 'W'
            {"ɜː", 1, 1},   // 'ɜ' -> 'o'
            {"l",  3, 3},   // 'l' -> 'l'
            {"d",  4, 4},   // 'd' -> 'd'
        },
    },
};

static const int example_count = sizeof(examples) / sizeof(examples[0]);

// ============================================================================
// Main Application
// ============================================================================

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "Formant TTS Circular Display Demo");
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "");

// LCD Pinout & SPI Configuration defaults (configurable via idf.py menuconfig)
#ifndef CONFIG_EXAMPLE_LCD_SPI_HOST
#define CONFIG_EXAMPLE_LCD_SPI_HOST 1  // SPI2_HOST
#endif
#ifndef CONFIG_EXAMPLE_LCD_MOSI_GPIO
#define CONFIG_EXAMPLE_LCD_MOSI_GPIO 28
#endif
#ifndef CONFIG_EXAMPLE_LCD_SCLK_GPIO
#define CONFIG_EXAMPLE_LCD_SCLK_GPIO 27
#endif
#ifndef CONFIG_EXAMPLE_LCD_CS_GPIO
#define CONFIG_EXAMPLE_LCD_CS_GPIO 24
#endif
#ifndef CONFIG_EXAMPLE_LCD_DC_GPIO
#define CONFIG_EXAMPLE_LCD_DC_GPIO 25
#endif
#ifndef CONFIG_EXAMPLE_LCD_RST_GPIO
#define CONFIG_EXAMPLE_LCD_RST_GPIO 26
#endif
#ifndef CONFIG_EXAMPLE_LCD_BL_GPIO
#define CONFIG_EXAMPLE_LCD_BL_GPIO -1
#endif
#ifndef CONFIG_EXAMPLE_LCD_SPI_CLOCK_HZ
#define CONFIG_EXAMPLE_LCD_SPI_CLOCK_HZ 40000000
#endif

    // ========================================
    // Display Hardware Configuration
    // ========================================
    formant_display_config_t config = {
        .spi_host = (spi_host_device_t)CONFIG_EXAMPLE_LCD_SPI_HOST,
        .pin_mosi = CONFIG_EXAMPLE_LCD_MOSI_GPIO,
        .pin_sclk = CONFIG_EXAMPLE_LCD_SCLK_GPIO,
        .pin_cs = CONFIG_EXAMPLE_LCD_CS_GPIO,
        .pin_dc = CONFIG_EXAMPLE_LCD_DC_GPIO,
        .pin_rst = CONFIG_EXAMPLE_LCD_RST_GPIO,
        .pin_bkl = CONFIG_EXAMPLE_LCD_BL_GPIO,
        .spi_clock_hz = CONFIG_EXAMPLE_LCD_SPI_CLOCK_HZ,
    };

    ESP_LOGI(TAG, "Display Hardware:");
    ESP_LOGI(TAG, "  MOSI: GPIO %d", config.pin_mosi);
    ESP_LOGI(TAG, "  SCLK: GPIO %d", config.pin_sclk);
    ESP_LOGI(TAG, "  CS:   GPIO %d", config.pin_cs);
    ESP_LOGI(TAG, "  DC:   GPIO %d", config.pin_dc);
    ESP_LOGI(TAG, "  RST:  GPIO %d", config.pin_rst);
    ESP_LOGI(TAG, "  BKL:  %s", config.pin_bkl < 0 ? "disabled" : "PWM enabled");
    ESP_LOGI(TAG, "");

    // ========================================
    // Initialize Display
    // ========================================
    ESP_LOGI(TAG, "Initializing display...");
    esp_err_t ret = formant_display_init(&config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "[ERROR] Display initialization failed: %s", esp_err_to_name(ret));
        return;
    }
    ESP_LOGI(TAG, "[OK] Display initialized");
    ESP_LOGI(TAG, "");

    // Set idle display text (shown when speech is paused)
    formant_display_set_idle_text("FORMANT TTS", "READY");

    // Show initial startup overlay with volume gauge (2 seconds)
    ESP_LOGI(TAG, "Showing initial volume HUD overlay (80%%)...");
    formant_display_show_volume_overlay(80, 2000);
    for (int i = 0; i < 60; i++) { // ~2 seconds @ 30 FPS
        formant_display_render();
        vTaskDelay(pdMS_TO_TICKS(33));
    }

    // ========================================
    // TTS Animation Loop
    // ========================================
    int example_idx = 0;
    int step_idx = 0;
    int frame = 0;
    float amplitude = 0.0f;

    ESP_LOGI(TAG, "Starting continuous TTS visualization loop...");
    ESP_LOGI(TAG, "");

    while (true) {
        const TTS_Example* current = &examples[example_idx];
        const PhonemeStep* step = &current->steps[step_idx];

        // Animate amplitude (speech-like envelope)
        amplitude = 0.65f + 0.35f * sinf(frame * 0.2f);
        if (amplitude < 0.0f) amplitude = 0.0f;
        if (amplitude > 1.0f) amplitude = 1.0f;

        // Update display with live TTS speech data
        formant_display_set_tts_data(
            amplitude,                          // Audio amplitude (0-1)
            current->word,                      // Current word for bottom arc
            step->ipa_symbol,                   // Current IPA phoneme for center display
            current->ipa,                       // Full IPA transcription for top arc
            step->word_char_idx,                // Word character index for bottom arc highlight
            step->ipa_char_idx                  // IPA character index for top arc highlight
        );

        // Render frame (30 FPS)
        formant_display_render();
        frame++;

        // Every 20 frames (~0.67s), advance to next phoneme
        if (frame % 20 == 0) {
            step_idx++;

            // End of word?
            if (step_idx >= current->step_count) {
                step_idx = 0;
                example_idx = (example_idx + 1) % example_count;

                ESP_LOGI(TAG, "Word complete. Next word: '%s' (IPA: '%s')",
                         examples[example_idx].word,
                         examples[example_idx].ipa);

                // Clear TTS data during inter-word silence to demonstrate idle transition
                formant_display_set_tts_data(0.0f, "", "sil", "", -1, -1);
                
                // Render silence/idle decay for 500ms
                for (int p = 0; p < 15; p++) {
                    formant_display_render();
                    vTaskDelay(pdMS_TO_TICKS(33));
                }
            } else {
                ESP_LOGI(TAG, "Word: '%s' | Phoneme: '%s' | IPA char: %d | Word char: %d",
                         current->word,
                         current->steps[step_idx].ipa_symbol,
                         current->steps[step_idx].ipa_char_idx,
                         current->steps[step_idx].word_char_idx);
            }
        }

        // Periodically demonstrate HUD volume adjustment overlay during speech
        if (frame % 600 == 0 && frame > 0) {
            int vol = 70 + ((frame / 600) % 5) * 5; // Cycle 70%..90%
            ESP_LOGI(TAG, "HUD Event: Volume set to %d%%", vol);
            formant_display_show_volume_overlay(vol, 1500);
        }

        // 30 FPS timing
        vTaskDelay(pdMS_TO_TICKS(33));
    }
}
