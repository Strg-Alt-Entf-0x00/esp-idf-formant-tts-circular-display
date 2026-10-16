/**
 * @file phoneme_renderer.cpp
 * @brief Phoneme display in center with UTF-8/IPA support
 */

#include "display_theme.h"
#include "fonts/font_config.h"
#include <string>
#include <cstring>

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

extern "C" void tts_phoneme_renderer_draw(void* sprite_ptr, const char* phoneme, float amplitude) {
    lgfx::LGFX_Sprite* sprite = static_cast<lgfx::LGFX_Sprite*>(sprite_ptr);
    if (!sprite || !phoneme) return;
    
    std::string phoneme_str(phoneme);
    if (phoneme_str.empty() || phoneme_str == "sil" || phoneme_str == "SIL") {
        return;
    }

    int lum = 100 + (int)(amplitude * 155.0f);
    if (lum > 255) lum = 255;
    uint16_t txt_color = lgfx::color565(lum, lum, lum);
    
    sprite->setTextDatum(lgfx::middle_center);
    sprite->setTextColor(txt_color);
    sprite->setFont(FONT_IPA_16);  // Use IPA-capable font (or FreeSans fallback)
    sprite->setTextSize(1.0f);     // Native 1:1 pixel rendering for 16pt font
    sprite->drawString(phoneme, theme::CX, theme::CY);  // u8g2 fonts handle UTF-8 automatically
}
