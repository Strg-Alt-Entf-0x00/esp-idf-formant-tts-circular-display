/**
 * @file spectrum_widget.cpp
 * @brief Stationary radial acoustic spectrum analyzer with formant resonance
 */

#include "widgets/spectrum_widget.h"
#include "display_theme.h"
#include <cmath>

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

void spectrum_widget_draw(void* sprite_ptr, float amplitude, int num_bars) {
    lgfx::LGFX_Sprite* sprite = static_cast<lgfx::LGFX_Sprite*>(sprite_ptr);
    if (!sprite) return;
    
    float max_bar_len = 34.0f;
    float inner_r = (float)theme::RADIUS_SPECTRUM_INNER + 2.0f; // ~22px
    
    // Amplitude smoothing: fast attack, smooth exponential decay
    static float smoothed_amp = 0.0f;
    if (amplitude > smoothed_amp) {
        smoothed_amp = amplitude; // Instant attack on phoneme onset
    } else {
        smoothed_amp *= 0.82f;    // Smooth acoustic decay
    }
    if (smoothed_amp < 0.01f) {
        smoothed_amp = 0.0f;
        return; // Inactive: clean display around phoneme
    }
    
    float visual_amp = sqrtf(smoothed_amp);
    if (visual_amp > 1.0f) visual_amp = 1.0f;
    
    float angle_step = (M_PI * 2.0f) / num_bars;
    
    // Draw stationary radial bars with speech formant acoustic weighting
    for (int i = 0; i < num_bars; i++) {
        // Fixed angle per frequency band (NO rotation)
        float angle_tts = i * angle_step - (M_PI / 2.0f);
        
        // Symmetrical formant envelope around circle
        // Bin index folded symmetrically so left and right sides balance
        float sym_i = (i < num_bars / 2) ? (float)i : (float)(num_bars - i);
        float norm_bin = sym_i / (num_bars / 2.0f); // 0.0 at top/bottom, 1.0 at sides
        
        // Formant peaks: F1 resonance at 0.3, F2 resonance at 0.75
        float f1 = expf(-powf((norm_bin - 0.30f) / 0.18f, 2.0f));
        float f2 = expf(-powf((norm_bin - 0.75f) / 0.22f, 2.0f));
        float resonance = 0.25f + 0.45f * f1 + 0.35f * f2;
        
        // Static harmonic texture (phase is fixed, absolutely NO spinning)
        float harmonic = 0.85f + 0.15f * sinf(sym_i * 2.2f);
        
        float bar_val = visual_amp * resonance * harmonic;
        if (bar_val < 0.05f) continue;
        if (bar_val > 1.0f) bar_val = 1.0f;
        
        float bar_len = bar_val * max_bar_len;
        
        int x1 = theme::CX + (int)(cosf(angle_tts) * inner_r);
        int y1 = theme::CY + (int)(sinf(angle_tts) * inner_r);
        int x2 = theme::CX + (int)(cosf(angle_tts) * (inner_r + bar_len));
        int y2 = theme::CY + (int)(sinf(angle_tts) * (inner_r + bar_len));
        
        // Harmonic rainbow spectrum hue anchored per bin
        float hue = norm_bin * 200.0f + 160.0f; // Electric Cyan (160°) -> Blue -> Magenta -> Rose (360°)
        if (hue > 360.0f) hue -= 360.0f;
        
        uint16_t color = theme::hsv_to_rgb565(hue, 0.9f, 0.95f);
        
        // Draw crisp 2px antialiased spike
        sprite->drawLine(x1, y1, x2, y2, color);
        
        // Highlight glowing tip for energetic beats
        if (bar_val > 0.45f) {
            sprite->drawPixel(x2, y2, lgfx::color565(255, 255, 255));
        }
    }
}
