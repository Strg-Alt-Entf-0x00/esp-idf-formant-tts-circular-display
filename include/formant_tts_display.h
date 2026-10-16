/**
 * @file formant_tts_display.h
 * @brief Main API for Formant TTS Circular Display
 * 
 * Circular display UI framework for real-time formant speech synthesis
 * visualization on 240x240 round displays.
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "driver/spi_master.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Display hardware configuration
 */
typedef struct {
    spi_host_device_t spi_host;     ///< SPI host (SPI2_HOST or SPI3_HOST)
    int pin_mosi;                    ///< SPI MOSI pin
    int pin_sclk;                    ///< SPI SCLK pin
    int pin_cs;                      ///< SPI CS pin
    int pin_dc;                      ///< Display DC (Data/Command) pin
    int pin_rst;                     ///< Display RESET pin
    int pin_bkl;                     ///< Backlight pin (-1 to disable)
    uint32_t spi_clock_hz;           ///< SPI clock speed (default: 40MHz)
} formant_display_config_t;

/**
 * @brief Display mode
 */
typedef enum {
    FORMANT_DISPLAY_MODE_TTS,        ///< Active mode: TTS visualization
    FORMANT_DISPLAY_MODE_CUSTOM      ///< Custom user-defined mode
} formant_display_mode_t;

/**
 * @brief Initialize the display
 * 
 * @param config Hardware configuration
 * @return ESP_OK on success
 */
esp_err_t formant_display_init(const formant_display_config_t* config);

/**
 * @brief Set display mode
 * 
 * @param mode Display mode (TTS or CUSTOM)
 */
void formant_display_set_mode(formant_display_mode_t mode);

/**
 * @brief Get current display mode
 * 
 * @return Current display mode
 */
formant_display_mode_t formant_display_get_mode(void);

/**
 * @brief Feed live TTS data (for TTS mode)
 * 
 * @param amplitude Audio amplitude (0.0 - 1.0)
 * @param word Current word being spoken
 * @param phoneme Current phoneme (Arpabet format, e.g., "AH")
 * @param ipa Full IPA string (e.g., "həˈloʊ")
 * @param word_idx Current character index in word (-1 for no highlight)
 * @param ipa_idx Current character index in IPA string (-1 for no highlight)
 */
void formant_display_set_tts_data(
    float amplitude,
    const char* word,
    const char* phoneme,
    const char* ipa,
    int word_idx,
    int ipa_idx
);

void formant_display_set_idle_text(const char* top_text, const char* bottom_text);

/**
 * @brief Callback function type for custom idle screen rendering
 * @param sprite_ptr Pointer to the 240x240 LGFX_Sprite
 */
typedef void (*formant_display_idle_renderer_t)(void* sprite_ptr);

/**
 * @brief Register a custom idle screen renderer (called when TTS is not active)
 * 
 * @param renderer Callback function or NULL to revert to default text arcs
 */
void formant_display_set_idle_renderer(formant_display_idle_renderer_t renderer);


/**
 * @brief Get underlying graphics sprite pointer (for custom drawing)
 * 
 * In FORMANT_DISPLAY_MODE_CUSTOM, this returns a pointer to the lgfx::LGFX_Sprite.
 * 
 * @return Pointer to sprite object
 */
void* formant_display_get_sprite(void);

/**
 * @brief Show volume overlay (HUD)
 * 
 * @param volume_pct Volume percentage (0-100)
 * @param duration_ms Display duration in milliseconds
 */
void formant_display_show_volume_overlay(int volume_pct, uint32_t duration_ms);

/**
 * @brief Set backlight brightness
 * 
 * @param brightness Brightness (0-100%)
 */
void formant_display_set_brightness(uint8_t brightness);

/**
 * @brief Clear the display
 */
void formant_display_clear(void);

/**
 * @brief Render current frame to display
 * 
 * Call this regularly (e.g., 30-60 FPS) to update the display.
 */
void formant_display_render(void);

/**
 * @brief Enable/disable test pattern (for debugging)
 * 
 * @param enable true to enable test pattern
 */
void formant_display_set_test_pattern(bool enable);

#ifdef __cplusplus
}
#endif
