/**
 * @file spectrum_widget.h
 * @brief Radial spectrum analyzer widget
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Draw radial spectrum bars
 * 
 * @param sprite LovyanGFX sprite pointer
 * @param amplitude Audio amplitude (0.0 - 1.0)
 * @param num_bars Number of bars (default: 32)
 */
void spectrum_widget_draw(void* sprite, float amplitude, int num_bars);

#ifdef __cplusplus
}
#endif
