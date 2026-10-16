/**
 * @file display_theme.h
 * @brief Theme constants for circular display (colors, radii, fonts)
 */

#pragma once

#include <stdint.h>

namespace theme {

// Display geometry
constexpr int DISPLAY_WIDTH = 240;
constexpr int DISPLAY_HEIGHT = 240;
constexpr int CX = 120;  // Center X
constexpr int CY = 120;  // Center Y

// Radii
constexpr int RADIUS_MAX = 120;
constexpr int RADIUS_OUTER = 115;        // Alphabet ring
constexpr int RADIUS_ARC = 68;           // IPA/Word arc
constexpr int RADIUS_SPECTRUM_INNER = 20; // Spectrum start
constexpr int RADIUS_SPECTRUM_OUTER = 60; // Spectrum end

// Colors (RGB565)
constexpr uint16_t COLOR_BG = 0x0000;           // Black
constexpr uint16_t COLOR_TEXT_ACTIVE = 0xFFFF;  // White
constexpr uint16_t COLOR_TEXT_IDLE = 0x7BEF;    // Gray
constexpr uint16_t COLOR_TEXT_DIM = 0x39E7;     // Dark gray

// Spectrum colors (generated via HSV)
constexpr float SPECTRUM_HUE_START = 90.0f;
constexpr float SPECTRUM_SATURATION = 0.9f;
constexpr float SPECTRUM_VALUE = 0.9f;

// Text sizes
constexpr float TEXT_SIZE_PHONEME = 0.9f;
constexpr float TEXT_SIZE_ARC = 0.7f;
constexpr float TEXT_SIZE_SCALE_ACTIVE = 0.85f;
constexpr float TEXT_SIZE_SCALE_INACTIVE = 0.55f;
constexpr float TEXT_SIZE_ALPHABET_ACTIVE = 0.9f;
constexpr float TEXT_SIZE_ALPHABET_INACTIVE = 0.6f;

// Animation
constexpr float SPECTRUM_SMOOTHING = 0.85f;    // Decay factor for spectrum
constexpr uint32_t TTS_IDLE_TIMEOUT_MS = 1000; // Hold TTS display for 1s after silence

/**
 * @brief Convert HSV to RGB565
 * @param h Hue (0-360)
 * @param s Saturation (0-1)
 * @param v Value (0-1)
 * @return RGB565 color
 */
uint16_t hsv_to_rgb565(float h, float s, float v);

} // namespace theme
