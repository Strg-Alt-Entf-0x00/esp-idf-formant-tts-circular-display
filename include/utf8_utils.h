/**
 * @file utf8_utils.h
 * @brief UTF-8 string handling utilities
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get number of bytes in UTF-8 character
 * @param str Pointer to UTF-8 string
 * @return Number of bytes (1-4), or 1 if invalid
 */
int utf8_char_bytes(const char* str);

/**
 * @brief Get number of UTF-8 characters in string
 * @param str UTF-8 string
 * @return Character count (not byte count!)
 */
int utf8_strlen(const char* str);

/**
 * @brief Get pointer to next UTF-8 character
 * @param str Current position in UTF-8 string
 * @return Pointer to next character
 */
const char* utf8_next_char(const char* str);

/**
 * @brief Extract single UTF-8 character
 * @param str Source string
 * @param out_char Output buffer
 * @param max_len Maximum buffer size
 */
void utf8_get_char(const char* str, char* out_char, int max_len);

/**
 * @brief Check if character is an IPA symbol
 * @param str UTF-8 character
 * @return true if IPA character
 */
bool utf8_is_ipa_char(const char* str);

#ifdef __cplusplus
}
#endif
