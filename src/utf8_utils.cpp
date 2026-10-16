/**
 * @file utf8_utils.cpp
 * @brief UTF-8 string handling utilities
 */

#include "utf8_utils.h"
#include <cstring>

int utf8_char_bytes(const char* str) {
    if (!str || !*str) return 0;
    
    unsigned char c = (unsigned char)*str;
    
    // ASCII (0xxxxxxx)
    if ((c & 0x80) == 0) return 1;
    
    // 2-byte UTF-8 (110xxxxx)
    if ((c & 0xE0) == 0xC0) return 2;
    
    // 3-byte UTF-8 (1110xxxx)
    if ((c & 0xF0) == 0xE0) return 3;
    
    // 4-byte UTF-8 (11110xxx)
    if ((c & 0xF8) == 0xF0) return 4;
    
    // Invalid UTF-8
    return 1; // Treat as single byte and skip
}

int utf8_strlen(const char* str) {
    if (!str) return 0;
    
    int count = 0;
    const char* p = str;
    
    while (*p) {
        int bytes = utf8_char_bytes(p);
        p += bytes;
        count++;
    }
    
    return count;
}

const char* utf8_next_char(const char* str) {
    if (!str || !*str) return str;
    
    int bytes = utf8_char_bytes(str);
    return str + bytes;
}

void utf8_get_char(const char* str, char* out_char, int max_len) {
    if (!str || !out_char || max_len < 1) return;
    
    int bytes = utf8_char_bytes(str);
    if (bytes > max_len - 1) bytes = max_len - 1;
    
    memcpy(out_char, str, bytes);
    out_char[bytes] = '\0';
}

bool utf8_is_ipa_char(const char* str) {
    if (!str || !*str) return false;
    
    unsigned char c1 = (unsigned char)str[0];
    
    // Check if this is a multi-byte UTF-8 character
    if ((c1 & 0x80) == 0) {
        // ASCII - not IPA
        return false;
    }
    
    if ((c1 & 0xE0) == 0xC0) {
        // 2-byte UTF-8
        if (!str[1]) return false;
        unsigned char c2 = (unsigned char)str[1];
        
        // Decode Unicode code point
        uint32_t codepoint = ((c1 & 0x1F) << 6) | (c2 & 0x3F);
        
        // IPA Extensions and Spacing Modifier Letters: U+0250–U+02FF (includes ˈ, ˌ, ː)
        if (codepoint >= 0x0250 && codepoint <= 0x02FF) return true;
        
        // Combining Diacriticals: U+0300–U+036F
        if (codepoint >= 0x0300 && codepoint <= 0x036F) return true;
        
        // Latin Extended-A: U+0100–U+017F
        if (codepoint >= 0x0100 && codepoint <= 0x017F) return true;

        // Latin-1 Supplement: U+00A0–U+00FF
        if (codepoint >= 0x00A0 && codepoint <= 0x00FF) return true;
    }
    
    return false;
}
