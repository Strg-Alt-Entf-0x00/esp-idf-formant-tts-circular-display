/**
 * @file word_tracker.cpp
 * @brief Word string arc with scrolling (bottom arc)
 */

#include "display_theme.h"
#include "widgets/curved_text_widget.h"

extern "C" void tts_word_tracker_draw(void* sprite_ptr, const char* word_text, int active_idx, float amplitude) {
    // Use curved text widget for word display on bottom arc
    curved_text_widget_draw(
        sprite_ptr,
        word_text,
        90.0f,               // Bottom center (90°)
        theme::RADIUS_ARC,   // Arc radius
        false,               // Counter-clockwise
        active_idx,
        amplitude
    );
}
