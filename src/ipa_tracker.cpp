/**
 * @file ipa_tracker.cpp
 * @brief IPA string arc with scrolling (top arc)
 */

#include "display_theme.h"
#include "widgets/curved_text_widget.h"

extern "C" void tts_ipa_tracker_draw(void* sprite_ptr, const char* ipa_text, int active_idx, float amplitude) {
    // Use curved text widget for IPA display on top arc
    curved_text_widget_draw(
        sprite_ptr,
        ipa_text,
        270.0f,              // Top center (270°)
        theme::RADIUS_ARC,   // Arc radius
        true,                // Clockwise
        active_idx,
        amplitude
    );
}
