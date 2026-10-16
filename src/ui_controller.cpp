/**
 * @file ui_controller.cpp
 * @brief Main UI controller (state machine, rendering logic)
 */

#include "formant_tts_display.h"
#include "display_theme.h"
#include "display_hal.h"
#include "internal_tts.h"
#include "widgets/spectrum_widget.h"
#include "widgets/curved_text_widget.h"
#include "esp_timer.h"
#include "esp_log.h"
#include <string>
#include <cstring>
#include <cmath>

#define LGFX_USE_V1
#include "fonts/font_config.h"

// UI State
static struct {
    formant_display_mode_t mode = FORMANT_DISPLAY_MODE_TTS;
    
    // TTS data
    float tts_amplitude;
    std::string tts_word;
    std::string tts_phoneme;
    std::string tts_ipa;
    int tts_word_idx = -1;
    int tts_ipa_idx = -1;
    uint32_t last_tts_update_ms;
    
    // Idle arc texts
    std::string idle_arc_top;
    std::string idle_arc_bottom;
    
    // HUD overlays
    int volume_pct;
    uint32_t volume_overlay_end_ms;
    
    bool test_pattern;
    bool last_tts_state;
    formant_display_idle_renderer_t idle_renderer = nullptr;
} s_state{};


void formant_display_set_mode(formant_display_mode_t mode) {
    s_state.mode = mode;
}

formant_display_mode_t formant_display_get_mode(void) {
    return s_state.mode;
}

void* formant_display_get_sprite(void) {
    return display_hal_get_sprite();
}

void formant_display_set_idle_text(const char* top_text, const char* bottom_text) {
    s_state.idle_arc_top = top_text ? top_text : "";
    s_state.idle_arc_bottom = bottom_text ? bottom_text : "";
}

void formant_display_set_idle_renderer(formant_display_idle_renderer_t renderer) {
    s_state.idle_renderer = renderer;
}


void formant_display_set_tts_data(
    float amplitude,
    const char* word,
    const char* phoneme,
    const char* ipa,
    int word_idx,
    int ipa_idx
) {
    s_state.tts_amplitude = amplitude;
    s_state.tts_word = word ? word : "";
    s_state.tts_phoneme = phoneme ? phoneme : "";
    s_state.tts_ipa = ipa ? ipa : "";
    s_state.tts_word_idx = word_idx;
    s_state.tts_ipa_idx = ipa_idx;
    s_state.last_tts_update_ms = esp_timer_get_time() / 1000;
}

void formant_display_show_volume_overlay(int volume_pct, uint32_t duration_ms) {
    s_state.volume_pct = volume_pct;
    if (s_state.volume_pct < 0) s_state.volume_pct = 0;
    if (s_state.volume_pct > 100) s_state.volume_pct = 100;
    s_state.volume_overlay_end_ms = (esp_timer_get_time() / 1000) + duration_ms;
}

void formant_display_set_test_pattern(bool enable) {
    s_state.test_pattern = enable;
}

void formant_display_render(void) {
    lgfx::LGFX_Sprite* sprite = static_cast<lgfx::LGFX_Sprite*>(display_hal_get_sprite());
    if (!sprite) return;
    
    // 1. Clear background
    sprite->fillSprite(theme::COLOR_BG);
    
    // Test pattern for debugging
    if (s_state.test_pattern) {
        uint16_t gridColor = lgfx::color565(30, 30, 30);
        for (int i = 0; i < theme::DISPLAY_WIDTH; i += 20) {
            sprite->drawLine(0, i, theme::DISPLAY_WIDTH, i, gridColor);
            sprite->drawLine(i, 0, i, theme::DISPLAY_HEIGHT, gridColor);
        }
        sprite->drawLine(theme::CX, 0, theme::CX, theme::DISPLAY_HEIGHT, lgfx::color565(60, 60, 60));
        sprite->drawLine(0, theme::CY, theme::DISPLAY_WIDTH, theme::CY, lgfx::color565(60, 60, 60));
        sprite->drawCircle(theme::CX, theme::CY, 40, lgfx::color565(40, 40, 40));
        sprite->drawCircle(theme::CX, theme::CY, 80, lgfx::color565(40, 40, 40));
        display_hal_push_frame();
        return;
    }
    
    uint32_t current_time_ms = esp_timer_get_time() / 1000;
    
    // 2. Render TTS visualization (in TTS mode)
    if (s_state.mode == FORMANT_DISPLAY_MODE_TTS) {
        // Check if TTS is actively receiving audio/phonemes
        if ((s_state.tts_amplitude > 0.05f) || 
            (!s_state.tts_phoneme.empty() && s_state.tts_phoneme != "sil" && s_state.tts_phoneme != "SIL")) {
            s_state.last_tts_update_ms = current_time_ms;
        }
        
        // Hold TTS display active for IDLE_TIMEOUT after last activity
        bool tts_active = (current_time_ms - s_state.last_tts_update_ms < theme::TTS_IDLE_TIMEOUT_MS);
        
        // Ghosting fix: clear sprite buffer on transition from speech to idle
        if (s_state.last_tts_state && !tts_active) {
            sprite->fillSprite(theme::COLOR_BG);
        }
        s_state.last_tts_state = tts_active;
        
        // Draw radial formant spectrum analyzer
        spectrum_widget_draw(sprite, s_state.tts_amplitude, 
                             CONFIG_FORMANT_DISPLAY_SPECTRUM_BARS);
        
        if (tts_active) {
            // TTS Active: Show center phoneme, IPA arc, word arc, and alphabet ring
            tts_phoneme_renderer_draw(sprite, s_state.tts_phoneme.c_str(), s_state.tts_amplitude);
            tts_ipa_tracker_draw(sprite, s_state.tts_ipa.c_str(), s_state.tts_ipa_idx, s_state.tts_amplitude);
            tts_word_tracker_draw(sprite, s_state.tts_word.c_str(), s_state.tts_word_idx, s_state.tts_amplitude);
            tts_alphabet_ring_draw(sprite, s_state.tts_phoneme.c_str(), s_state.tts_word.c_str(), s_state.tts_word_idx);
        } else {
            // TTS Idle: Show custom idle screen or default idle arcs
            if (s_state.idle_renderer) {
                s_state.idle_renderer(sprite);
            } else {
                if (!s_state.idle_arc_top.empty()) {
                    curved_text_widget_draw(sprite, s_state.idle_arc_top.c_str(), 270.0f, 
                                          theme::RADIUS_ARC, true, -1, 0.0f);
                }
                if (!s_state.idle_arc_bottom.empty()) {
                    curved_text_widget_draw(sprite, s_state.idle_arc_bottom.c_str(), 90.0f, 
                                          theme::RADIUS_ARC, false, -1, 0.0f);
                }
            }
        }

    }
    
    // 3. HUD Overlays (volume level gauge)
    bool draw_vol = (current_time_ms <= s_state.volume_overlay_end_ms);
    if (draw_vol) {
        // Subtle dimming overlay (checkerboard pattern)
        for (int y = 0; y < theme::DISPLAY_HEIGHT; y++) {
            for (int x = (y % 2); x < theme::DISPLAY_WIDTH; x += 2) {
                sprite->drawPixel(x, y, 0x0000);
            }
        }
        
        int r_outer = theme::RADIUS_MAX - 12;
        int r_inner = theme::RADIUS_MAX - 16;
        float r_mid = (r_outer + r_inner) / 2.0f;
        float cap_r = 12.0f;
        uint16_t track_bg_color = lgfx::color565(30, 30, 30);
        uint16_t cap_bg_color = lgfx::color565(60, 60, 60);
        
        auto get_grad_color = [](float progress) -> uint16_t {
            int r = 0, g = 0;
            if (progress < 0.5f) {
                r = (int)(progress * 2.0f * 255.0f);
                g = 255;
            } else {
                r = 255;
                g = (int)((1.0f - (progress - 0.5f) * 2.0f) * 255.0f);
            }
            return lgfx::color565(r, g, 0);
        };
        
        // Volume arc (left side, 100° to 260°)
        int total_arc = 260 - 100;
        int angle_end = 100 + (int)((s_state.volume_pct / 100.0f) * total_arc);
        
        if (s_state.volume_pct > 0) {
            sprite->fillArc(theme::CX, theme::CY, r_outer + 2, r_inner - 2, 100, 260, track_bg_color);
            
            for (int a = 100; a < angle_end; a += 3) {
                int a_end = (a + 3 > angle_end) ? angle_end : (a + 3);
                float mid_a = (a + a_end) / 2.0f;
                float progress = (mid_a - 100.0f) / total_arc;
                sprite->fillArc(theme::CX, theme::CY, r_outer, r_inner, a, a_end, get_grad_color(progress));
            }
            
            // Start cap ("O")
            float rad = 100.0f * M_PI / 180.0f;
            int cx = theme::CX + (int)(cosf(rad) * r_mid);
            int cy = theme::CY + (int)(sinf(rad) * r_mid);
            sprite->fillCircle(cx, cy, (int)cap_r, cap_bg_color);
            sprite->setTextDatum(lgfx::middle_center);
            sprite->setFont(&fonts::Font0);
            sprite->setTextSize(1.0f);
            sprite->setTextColor(lgfx::color565(255, 255, 255));
            sprite->drawString("O", cx, cy);
            
            // End cap (volume percentage)
            rad = angle_end * M_PI / 180.0f;
            cx = theme::CX + (int)(cosf(rad) * r_mid);
            cy = theme::CY + (int)(sinf(rad) * r_mid);
            sprite->fillCircle(cx, cy, (int)cap_r, cap_bg_color);
            char vol_str[8];
            snprintf(vol_str, sizeof(vol_str), "%d%%", s_state.volume_pct);
            sprite->drawString(vol_str, cx, cy);
        }
    }
    
    // 4. Push frame to display
    display_hal_push_frame();
}
