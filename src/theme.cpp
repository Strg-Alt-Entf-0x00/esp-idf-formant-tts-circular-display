/**
 * @file theme.cpp
 * @brief Theme implementation (color conversions, utilities)
 */

#include "display_theme.h"
#include <cmath>

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

namespace theme {

/**
 * @brief Convert HSV to RGB565 using LovyanGFX
 * @param h Hue (0-360)
 * @param s Saturation (0-1)
 * @param v Value (0-1)
 * @return RGB565 color
 */
uint16_t hsv_to_rgb565(float h, float s, float v) {
    float h_mod = fmod(h, 360.0f);
    if (h_mod < 0) h_mod += 360.0f;
    
    int hi = (int)(h_mod / 60.0f) % 6;
    float f = (h_mod / 60.0f) - (int)(h_mod / 60.0f);
    float p = v * (1.0f - s);
    float q = v * (1.0f - f * s);
    float t = v * (1.0f - (1.0f - f) * s);
    
    float r = 0, g = 0, b = 0;
    switch (hi) {
        case 0: r=v; g=t; b=p; break;
        case 1: r=q; g=v; b=p; break;
        case 2: r=p; g=v; b=t; break;
        case 3: r=p; g=q; b=v; break;
        case 4: r=t; g=p; b=v; break;
        case 5: r=v; g=p; b=q; break;
    }
    
    uint8_t r8 = (uint8_t)(r * 255.0f);
    uint8_t g8 = (uint8_t)(g * 255.0f);
    uint8_t b8 = (uint8_t)(b * 255.0f);
    
    return lgfx::color565(r8, g8, b8);
}

} // namespace theme
