/**
 * @file curved_text_widget.cpp
 * @brief Curved text on arcs implementation with UTF-8/IPA support
 */

#include "widgets/curved_text_widget.h"
#include "display_theme.h"
#include "fonts/font_config.h"
#include "utf8_utils.h"
#include <cmath>
#include <vector>
#include <string>

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

void curved_text_widget_draw(
    void* sprite_ptr,
    const char* text,
    float center_angle_deg,
    int radius,
    bool clockwise,
    int active_idx,
    float amplitude
) {
    lgfx::LGFX_Sprite* sprite = static_cast<lgfx::LGFX_Sprite*>(sprite_ptr);
    if (!sprite || !text) return;
    
    std::string text_str(text);
    if (text_str.empty()) return;
    
    lgfx::LGFX_Sprite char_spr(sprite);
    char_spr.setColorDepth(16);
    char_spr.createSprite(32, 32);
    char_spr.setTextDatum(lgfx::middle_center);
    char_spr.setFont(FONT_IPA_12);  // Use IPA-capable font (or FreeSans fallback)

    // Calculate UTF-8 string length (number of characters, not bytes)
    int utf8_len = utf8_strlen(text);
    
    auto get_char_angle = [&](const char* utf8_char) -> float {
        float w = char_spr.textWidth(utf8_char);
        if (utf8_char[0] == ' ') w = char_spr.textWidth("a");
        float padding = 7.0f; // Generous letter spacing for clear readability
        return ((w + padding) / radius) * (180.0f / M_PI);
    };
    
    bool scrolling = (active_idx >= 0) && (utf8_len > 14);
    
    auto draw_char_styled = [&](const char* p, int idx, float current_angle) {
        if (!p || !*p) return;
        if (idx < 0 || idx >= utf8_len) return;
        
        // Extract single UTF-8 character
        char utf8_char[5] = {0};
        utf8_get_char(p, utf8_char, sizeof(utf8_char));
        
        if (utf8_char[0] == ' ') return;
        
        uint16_t color;
        float zoom = 1.0f;
        
        if (active_idx == -1) {
            color = theme::COLOR_TEXT_IDLE; // Idle base color (Gray)
        } else if (idx < active_idx) {
            color = lgfx::color565(150, 150, 150); // Past (Light Gray)
        } else if (idx == active_idx) {
            color = theme::COLOR_TEXT_ACTIVE; // Present (Pure White)
            zoom = 1.3f + (amplitude * 0.2f); // Elegant emphasis zoom
        } else {
            color = theme::COLOR_TEXT_DIM; // Future (Dark Gray)
        }
        
        char_spr.fillSprite(lgfx::color565(1,1,1));
        char_spr.setTextSize(1.0f);  // Native 1:1 pixel rendering for 12pt font
        char_spr.setTextColor(color, lgfx::color565(1,1,1));
        char_spr.drawString(utf8_char, 16, 16);  // u8g2 fonts handle UTF-8 automatically
        
        float rad = current_angle * M_PI / 180.0f;
        int cx = theme::CX + (int)(cosf(rad) * radius); 
        int cy = theme::CY + (int)(sinf(rad) * radius);
        
        float rot = clockwise ? (current_angle + 90.0f) : (current_angle - 90.0f);
        char_spr.pushRotateZoom(sprite, cx, cy, rot, zoom, zoom, lgfx::color565(1,1,1));
    };
    
    if (!scrolling) {
        // Non-scrolling: center the entire string
        float total_angle_span = 0;
        std::vector<float> char_angles;
        std::vector<const char*> char_positions;
        
        const char* p = text;
        for (int i = 0; i < utf8_len; i++) {
            char utf8_char[5] = {0};
            utf8_get_char(p, utf8_char, sizeof(utf8_char));
            
            float a = get_char_angle(utf8_char);
            char_angles.push_back(a);
            char_positions.push_back(p);
            total_angle_span += a;
            
            p = utf8_next_char(p);
        }
        
        float start_angle = center_angle_deg - (total_angle_span / 2.0f) * (clockwise ? 1 : -1);
        float current_angle = start_angle;
        
        for (int i = 0; i < utf8_len; i++) {
            float char_angle_step = char_angles[i] * (clockwise ? 1 : -1);
            current_angle += char_angle_step / 2.0f;
            draw_char_styled(char_positions[i], i, current_angle);
            current_angle += char_angle_step / 2.0f;
        }
    } else {
        // Scrolling: center the active character
        int center_idx = active_idx;
        if (center_idx < 0) center_idx = 0;
        if (center_idx >= utf8_len) center_idx = utf8_len - 1;
        
        // Build character position map
        std::vector<const char*> char_positions;
        const char* p = text;
        for (int i = 0; i < utf8_len; i++) {
            char_positions.push_back(p);
            p = utf8_next_char(p);
        }
        
        // Draw center character
        char utf8_center[5] = {0};
        utf8_get_char(char_positions[center_idx], utf8_center, sizeof(utf8_center));
        float center_char_a = get_char_angle(utf8_center);
        draw_char_styled(char_positions[center_idx], center_idx, center_angle_deg);
        
        // Draw characters to the right
        float right_angle = center_angle_deg + (center_char_a / 2.0f) * (clockwise ? 1 : -1);
        for (int i = center_idx + 1; i < utf8_len; i++) {
            char utf8_char[5] = {0};
            utf8_get_char(char_positions[i], utf8_char, sizeof(utf8_char));
            float a = get_char_angle(utf8_char);
            
            right_angle += (a / 2.0f) * (clockwise ? 1 : -1);
            
            float diff = fmod(std::abs(right_angle - center_angle_deg), 360.0f);
            if (diff > 180.0f) diff = 360.0f - diff;
            if (diff > 50.0f) break; 
            
            draw_char_styled(char_positions[i], i, right_angle);
            right_angle += (a / 2.0f) * (clockwise ? 1 : -1);
        }
        
        // Draw characters to the left
        float left_angle = center_angle_deg - (center_char_a / 2.0f) * (clockwise ? 1 : -1);
        for (int i = center_idx - 1; i >= 0; i--) {
            char utf8_char[5] = {0};
            utf8_get_char(char_positions[i], utf8_char, sizeof(utf8_char));
            float a = get_char_angle(utf8_char);
            
            left_angle -= (a / 2.0f) * (clockwise ? 1 : -1);
            
            float diff = fmod(std::abs(left_angle - center_angle_deg), 360.0f);
            if (diff > 180.0f) diff = 360.0f - diff;
            if (diff > 50.0f) break; 
            
            draw_char_styled(char_positions[i], i, left_angle);
            left_angle -= (a / 2.0f) * (clockwise ? 1 : -1);
        }
    }
    char_spr.deleteSprite();
}
