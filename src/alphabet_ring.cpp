/**
 * @file alphabet_ring.cpp
 * @brief Outer alphabet ring A-Z with glowing enlarged active letter
 */

#include "display_theme.h"
#include <string>
#include <cctype>
#include <cstring>

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

#include "fonts/font_config.h"

static const char* ALPHABET[] = {"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z"};
static const int ALPHABET_COUNT = 26;

extern "C" void tts_alphabet_ring_draw(void* sprite_ptr, const char* active_phoneme, const char* word, int word_idx) {
    lgfx::LGFX_Sprite* sprite = static_cast<lgfx::LGFX_Sprite*>(sprite_ptr);
    if (!sprite) return;
    
    sprite->setTextDatum(lgfx::middle_center);
    
    // Determine active character
    char active_char = ' ';
    if (word && word_idx >= 0 && word_idx < (int)strlen(word)) {
        char c = word[word_idx];
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            active_char = (char)toupper((unsigned char)c);
        }
    }
    
    if (active_char == ' ' && active_phoneme && strlen(active_phoneme) > 0) {
        std::string phoneme_str(active_phoneme);
        if (phoneme_str != "sil" && phoneme_str != "SIL") {
            char c = phoneme_str[0];
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
                active_char = (char)toupper((unsigned char)c);
            }
        }
    }
    
    int active_idx = -1;
    
    // 1. Draw inactive letters (10pt native font, subtle dim color)
    sprite->setFont(FONT_SCALE);
    sprite->setTextColor(theme::COLOR_TEXT_DIM);
    
    for (int i = 0; i < ALPHABET_COUNT; i++) {
        if (ALPHABET[i][0] == active_char) {
            active_idx = i;
            continue; // Drawn in pass 2 with enlargement and halo
        }
        
        float angle = ((float)i / ALPHABET_COUNT) * M_PI * 2.0f - (M_PI / 2.0f);
        int x = theme::CX + (int)(cosf(angle) * (theme::RADIUS_OUTER - 5));
        int y = theme::CY + (int)(sinf(angle) * (theme::RADIUS_OUTER - 5));
        sprite->drawString(ALPHABET[i], x, y);
    }
    
    // 2. Draw active letter with glowing highlight and larger 12pt font
    if (active_idx >= 0) {
        float angle = ((float)active_idx / ALPHABET_COUNT) * M_PI * 2.0f - (M_PI / 2.0f);
        int x = theme::CX + (int)(cosf(angle) * (theme::RADIUS_OUTER - 5));
        int y = theme::CY + (int)(sinf(angle) * (theme::RADIUS_OUTER - 5));
        
        // Illuminated highlight badge
        sprite->fillCircle(x, y, 9, lgfx::color565(20, 50, 90));
        sprite->drawCircle(x, y, 9, lgfx::color565(0, 200, 255));
        
        // Larger font for active letter (12pt instead of 10pt)
        sprite->setFont(FONT_IPA_12);
        sprite->setTextColor(lgfx::color565(255, 255, 255));
        sprite->drawString(ALPHABET[active_idx], x, y);
        
        // Restore font
        sprite->setFont(FONT_SCALE);
    }
}
