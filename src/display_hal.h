/**
 * @file display_hal.h
 * @brief Internal display hardware abstraction declarations
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get pointer to underlying LGFX sprite
 * @return Pointer to LGFX_Sprite instance
 */
void* display_hal_get_sprite(void);

/**
 * @brief Push the current framebuffer to the LCD panel via SPI DMA
 */
void display_hal_push_frame(void);

#ifdef __cplusplus
}
#endif
