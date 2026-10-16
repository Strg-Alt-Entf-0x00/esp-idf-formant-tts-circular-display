/**
 * @file internal_tts.h
 * @brief Internal TTS renderer function declarations
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Draw phoneme in center
 */
void tts_phoneme_renderer_draw(void* sprite, const char* phoneme, float amplitude);

/**
 * @brief Draw alphabet ring A-Z
 */
void tts_alphabet_ring_draw(void* sprite, const char* active_phoneme, const char* word, int word_idx);

/**
 * @brief Draw IPA string on top arc
 */
void tts_ipa_tracker_draw(void* sprite, const char* ipa_text, int active_idx, float amplitude);

/**
 * @brief Draw word string on bottom arc
 */
void tts_word_tracker_draw(void* sprite, const char* word_text, int active_idx, float amplitude);

#ifdef __cplusplus
}
#endif
