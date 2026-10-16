/**
 * @file curved_text_widget.h
 * @brief Curved text rendering on circular arcs
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Draw text on a circular arc
 * 
 * @param sprite LovyanGFX sprite pointer
 * @param text Text string to render
 * @param center_angle_deg Center angle in degrees (0° = top)
 * @param radius Radius from center
 * @param clockwise true for clockwise text, false for counter-clockwise
 * @param active_idx Character index to highlight (-1 for none)
 * @param amplitude Animation amplitude (0.0 - 1.0)
 */
void curved_text_widget_draw(
    void* sprite,
    const char* text,
    float center_angle_deg,
    int radius,
    bool clockwise,
    int active_idx,
    float amplitude
);

#ifdef __cplusplus
}
#endif
