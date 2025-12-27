#ifndef _STRINGHELPER_HPP_
#define _STRINGHELPER_HPP_

#include <string>
#include <cstdint>

class CStringHelper {
public:
    /**
     * @brief Remove the first matched sub-string
     *
     * Find from particular position and remove the first matched sub-string
     * If there're no sub-strings found, the destination parameter is untouched
     *
     * @param rstr_Dest - The destination of output, keep untouched if errors occur
     * @param krstr_Src - The source
     * @param krstr_SubStr - The substring
     * @param ui32_StartPos - The start finding position in the source
     * @return An uint32_t number - The position of first found sub-string in the source
     */
    static uint32_t fui32_Remove(std::string& rstr_Dest,
                                 const std::string& krstr_Src,
                                 const std::string& krstr_SubStr,
                                 uint32_t ui32_StartPos = 0U);
    /**
     * @brief Remove all matched sub-string
     *
     * Find and remove all the sub-string
     *
     * @param krstr_Src - The source
     * @param krstr_SubStr - The substring
     *
     * @return The string which all sub-string were removed
     */
    static std::string fstr_RemoveAll(const std::string& krstr_Src, const std::string& krstr_SubStr);

    /**
     * @brief Remove the first matched sub-string
     *
     * Find from particular position and replace the first sub-string
     * If there're no sub-strings found, the destination parameter is untouched
     *
     * @param rstr_Dest - The destination of output, keep untouched if errors occur
     * @param krstr_Src - The source
     * @param krstr_SubStr - The substring
     * @param krstr_Replacement - The replacement for substring
     * @param ui32_StartPos - The start finding position in the source
     *
     * @return An uint32_t number - The position of first found sub-string in the source
     */
    static uint32_t fui32_Repace(std::string& rstr_Dest,
                                 const std::string& krstr_Src,
                                 const std::string& krstr_SubStr,
                                 const std::string& krstr_Replacement,
                                 uint32_t ui32_StartPos = 0U);

    /**
     * @brief Replace all matched sub-string
     *
     * Find and remove all the sub-string
     *
     * @param krstr_Src - The source
     * @param krstr_SubStr - The substring
     * @param krstr_Replacement - The replacement for substring
     *
     * @return A std::string - Sub-strings replaced result
     */
    static std::string fstr_RepaceAll(const std::string& krstr_Src,
                                      const std::string& krstr_SubStr,
                                      const std::string& krstr_Replacement);

    /**
     * @brief Remove inplace all paticular character on the left of the string
     *
     * @param rstr - The source
     * @param c - The character to be removed
     *
     * @return NONE
     */
    static void fv_LeftTrim(std::string& rstr, char c);

    /**
     * @brief Remove inplace all paticular character on the right of the string
     *
     * @param rstr - The source
     * @param c - The character to be removed
     *
     * @return None
     */
    static void fv_RightTrim(std::string& rstr, char c);

    /**
     * @brief Remove inplace all paticular character on both side of the string
     *
     * @param rstr - The source
     * @param c - The character to be removed
     *
     * @return None
     */
    static void fv_Trim(std::string& rstr, char c);


    /**
     * @brief Create a string from "printf" format
     *
     * @param kstr_Format - The format follow style of "printf"
     * @param ... - Other parameters
     *
     * @return A std::string - The formatted string
     */
    static std::string fstr_Format(const std::string& kstr_Format, ...);

    /**
     * @brief Create a string inplace from "printf" format
     *
     * @param rstr_Dest - The destination of output, keep untouched if errors occur
     * @param kstr_Format - The format follow style of "printf"
     * @param ... - Other parameters
     *
     * @return None
     */
    static void fv_Format(std::string& rstr_Dest, const std::string& kstr_Format, ...);
};

#endif
