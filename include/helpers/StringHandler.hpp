#ifndef CPPNEOFETCH_STRING_HANDLER_HPP
#define CPPNEOFETCH_STRING_HANDLER_HPP

#include "utils/Definitions.hpp"

#include <cstddef>
#include <string>
#include <string_view>

/**
 * @brief Utility functions for string trimming, replacement, extraction, and formatting.
 */
namespace StringHandler {

/**
 * @brief Remove leading characters matching any character in krsv_Chars in place.
 *
 * @param rstr_Target - The string to modify in place
 * @param krsv_Chars - Set of characters to strip from the left (defaults to whitespace)
 */
void fv_LeftTrim(std::string& rstr_Target,
                 const std::string_view& krsv_Chars = Chars::ksv_Whitespace);

/**
 * @brief Remove trailing characters matching any character in krsv_Chars in place.
 *
 * @param rstr_Target - The string to modify in place
 * @param krsv_Chars - Set of characters to strip from the right (defaults to whitespace)
 */
void fv_RightTrim(std::string& rstr_Target,
                  const std::string_view& krsv_Chars = Chars::ksv_Whitespace);

/**
 * @brief Remove both leading and trailing characters matching krsv_Chars in place.
 *
 * @param rstr_Target - The string to modify in place
 * @param krsv_Chars - Set of characters to strip from both ends (defaults to whitespace)
 */
void fv_Trim(std::string& rstr_Target,
             const std::string_view& krsv_Chars = Chars::ksv_Whitespace);

/**
 * @brief Remove leading characters matching any character in krsv_Chars in place.
 *
 * @param rstr_Target - The string_view to modify in place
 * @param krsv_Chars - Set of characters to strip from the left (defaults to whitespace)
 */
void fv_LeftTrimStrView(std::string_view& rstr_Target,
                        const std::string_view& krsv_Chars = Chars::ksv_Whitespace);

/**
 * @brief Remove trailing characters matching any character in krsv_Chars in place.
 *
 * @param rstr_Target - The string_view to modify in place
 * @param krsv_Chars - Set of characters to strip from the right (defaults to whitespace)
 */
void fv_RightTrimStrView(std::string_view& rstr_Target,
                         const std::string_view& krsv_Chars = Chars::ksv_Whitespace);

/**
 * @brief Remove both leading and trailing characters matching krsv_Chars in place.
 *
 * @param rstr_Target - The string_view to modify in place
 * @param krsv_Chars - Set of characters to strip from both ends (defaults to whitespace)
 */
void fv_TrimStrView(std::string_view& rstr_Target,
                    const std::string_view& krsv_Chars = Chars::ksv_Whitespace);

/**
 * @brief Replace all occurrences of a substring within a source string.
 *
 * @param krsv_Src - The source string view
 * @param sv_From - The substring to search for
 * @param sv_To - The replacement substring
 *
 * @return A new std::string with all occurrences replaced
 */
std::string fstr_ReplaceAll(const std::string_view& krsv_Src,
                            const std::string_view& krsv_From,
                            const std::string_view& krsv_To);

/**
 * @brief Extract content inside the first balanced pair of delimiter characters.
 *
 * @param krsv_Src - The source string view
 * @param c_Start - Opening delimiter character (defaults to '[')
 * @param c_End - Closing delimiter character (defaults to ']')
 *
 * @return Extracted substring, or empty string if no balanced pair is found
 */
std::string fstr_GetSubstringBetween(const std::string_view& krsv_Src,
                                     char c_Start = '[',
                                     char c_End = ']');

/**
 * @brief Format a string from a printf-style format string without VLAs.
 *
 * @param kpc_Format - printf-style format string
 * @param ... - Variadic arguments matching the format specifiers
 *
 * @return Formatted std::string, or empty string on error
 */
std::string fstr_Format(const char* kpc_Format, ...)
    __attribute__((format(printf, 1, 2)));

/**
 * @brief Count terminal display columns (UTF-8 codepoints) of a UTF-8 string.
 *
 * @param krsv_Text - Input UTF-8 string view
 *
 * @return Number of displayable codepoints
 */
std::size_t fui64_Utf8DisplayWidth(const std::string_view& krsv_Text);

} // namespace StringHandler

#endif // CPPNEOFETCH_STRING_HANDLER_HPP
