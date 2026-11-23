#include "helpers/StringHandler.hpp"

#include <cstdarg>
#include <cstdio>

namespace StringHandler {

namespace {

constexpr unsigned char kuc_Utf8ContMask = 0xC0U;
constexpr unsigned char kuc_Utf8ContVal  = 0x80U;

} // namespace

/**
 * @brief Remove leading characters matching any character in krsv_Chars in place.
 *
 * @param rstr_Target - The string to modify in place
 * @param krsv_Chars - Set of characters to strip from the left (defaults to whitespace)
 */
void fv_LeftTrim(std::string& rstr_Target, const std::string_view& krsv_Chars) {
    const std::size_t ui64_Pos = rstr_Target.find_first_not_of(krsv_Chars);
    if (std::string::npos == ui64_Pos) {
        rstr_Target.clear();
    } else if (0U < ui64_Pos) {
        rstr_Target.erase(0U, ui64_Pos);
    }
}

/**
 * @brief Remove trailing characters matching any character in krsv_Chars in place.
 *
 * @param rstr_Target - The string to modify in place
 * @param krsv_Chars - Set of characters to strip from the right (defaults to whitespace)
 */
void fv_RightTrim(std::string& rstr_Target, const std::string_view& krsv_Chars) {
    const std::size_t ui64_Pos = rstr_Target.find_last_not_of(krsv_Chars);
    if (std::string::npos == ui64_Pos) {
        rstr_Target.clear();
    } else {
        rstr_Target.erase(ui64_Pos + 1U);
    }
}

/**
 * @brief Remove both leading and trailing characters matching krsv_Chars in place.
 *
 * @param rstr_Target - The string to modify in place
 * @param krsv_Chars - Set of characters to strip from both ends (defaults to whitespace)
 */
void fv_Trim(std::string& rstr_Target, const std::string_view& krsv_Chars) {
    fv_RightTrim(rstr_Target, krsv_Chars);
    fv_LeftTrim(rstr_Target, krsv_Chars);
}

/**
 * @brief Remove leading characters matching any character in krsv_Chars in place.
 *
 * @param rstr_Target - The string_view to modify in place
 * @param krsv_Chars - Set of characters to strip from the left (defaults to whitespace)
 */
void fv_LeftTrimStrView(std::string_view& rstr_Target, const std::string_view& krsv_Chars) {
    const std::size_t ui64_Pos = rstr_Target.find_first_not_of(krsv_Chars);
    if (std::string_view::npos == ui64_Pos) {
        rstr_Target = std::string_view{};
    } else {
        rstr_Target.remove_prefix(ui64_Pos);
    }
}

/**
 * @brief Remove trailing characters matching any character in krsv_Chars in place.
 *
 * @param rstr_Target - The string_view to modify in place
 * @param krsv_Chars - Set of characters to strip from the right (defaults to whitespace)
 */
void fv_RightTrimStrView(std::string_view& rstr_Target, const std::string_view& krsv_Chars) {
    const std::size_t ui64_Pos = rstr_Target.find_last_not_of(krsv_Chars);
    if (std::string_view::npos == ui64_Pos) {
        rstr_Target = std::string_view{};
    } else {
        rstr_Target.remove_suffix(rstr_Target.size() - ui64_Pos - 1);
    }
}

/**
 * @brief Remove both leading and trailing characters matching krsv_Chars in place.
 *
 * @param rstr_Target - The string_view to modify in place
 * @param krsv_Chars - Set of characters to strip from both ends (defaults to whitespace)
 */
void fv_TrimStrView(std::string_view& rstr_Target, const std::string_view& krsv_Chars) {
    fv_RightTrimStrView(rstr_Target, krsv_Chars);
    fv_LeftTrimStrView(rstr_Target, krsv_Chars);
}

/**
 * @brief Replace all occurrences of a substring within a source string.
 *
 * @param krsv_Src - The source string view
 * @param krsv_From - The substring to search for
 * @param krsv_To - The replacement substring
 *
 * @return A new std::string with all occurrences replaced
 */
std::string fstr_ReplaceAll(const std::string_view& krsv_Src,
                            const std::string_view& krsv_From,
                            const std::string_view& krsv_To) {
    if (krsv_From.empty()) {
        return std::string(krsv_Src);
    }
    std::string str_Result;
    str_Result.reserve(krsv_Src.size());
    std::size_t ui64_Pos = 0U;
    while (ui64_Pos < krsv_Src.size()) {
        const std::size_t ui64_Found = krsv_Src.find(krsv_From, ui64_Pos);
        if (std::string_view::npos == ui64_Found) {
            str_Result.append(krsv_Src.substr(ui64_Pos));
            break;
        }
        str_Result.append(krsv_Src.substr(ui64_Pos, ui64_Found - ui64_Pos));
        str_Result.append(krsv_To);
        ui64_Pos = ui64_Found + krsv_From.size();
    }
    return str_Result;
}

/**
 * @brief Extract content inside the first and the last pair of specific characters.
 *
 * @param krsv_Src - The source string view
 * @param c_Start - Opening delimiter character (defaults to '[')
 * @param c_End - Closing delimiter character (defaults to ']')
 *
 * @return Extracted substring, or empty string if no pair is found
 */
std::string fstr_GetSubstringBetween(const std::string_view& krsv_Src,
                                     char c_Start,
                                     char c_End) {
    const std::size_t ui64_Start = krsv_Src.find(c_Start);
    if (std::string_view::npos == ui64_Start) {
        return {};
    }

    std::size_t ui64_LastCharEndIdx = 0;
    for (std::size_t ui64_Idx = ui64_Start; ui64_Idx < krsv_Src.size(); ++ui64_Idx) {
        if (c_End == krsv_Src[ui64_Idx]) {
            ui64_LastCharEndIdx = ui64_Idx;
        }
    }

    if (ui64_Start + 1U < ui64_LastCharEndIdx) {
        return std::string(krsv_Src.substr(ui64_Start + 1U, ui64_LastCharEndIdx - ui64_Start - 1U));
    }

    return {};
}

/**
 * @brief Format a string from a printf-style format string without VLAs.
 *
 * @param kpc_Format - printf-style format string
 * @param ... - Variadic arguments matching the format specifiers
 *
 * @return Formatted std::string, or empty string on error
 */
std::string fstr_Format(const char* kpc_Format, ...) {
    if (nullptr == kpc_Format) {
        return {};
    }
    va_list args;
    va_start(args, kpc_Format);
    va_list argsCopy;
    va_copy(argsCopy, args);

    const int i32_Needed = std::vsnprintf(nullptr, 0, kpc_Format, args);
    va_end(args);

    if (0 >= i32_Needed) {
        va_end(argsCopy);
        return {};
    }

    std::string str_Result(static_cast<std::size_t>(i32_Needed), '\0');
    std::vsnprintf(str_Result.data(), str_Result.size() + 1U, kpc_Format, argsCopy);
    va_end(argsCopy);
    return str_Result;
}

/**
 * @brief Count terminal display columns (UTF-8 codepoints) of a UTF-8 string.
 *
 * @param krsv_Text - Input UTF-8 string view
 *
 * @return Number of displayable codepoints
 */
std::size_t fui64_Utf8DisplayWidth(const std::string_view& krsv_Text) {
    std::size_t ui64_Width = 0U;
    for (unsigned char uc_Ch : krsv_Text) {
        if (kuc_Utf8ContVal != (uc_Ch & kuc_Utf8ContMask)) {
            ++ui64_Width;
        }
    }
    return ui64_Width;
}

} // namespace StringHandler
