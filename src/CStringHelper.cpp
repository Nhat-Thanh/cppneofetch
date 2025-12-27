#include "CStringHelper.hpp"
#include "Definitions.hpp"
#include <algorithm>
#include <cstdint>
#include <cstdarg>

uint32_t CStringHelper::fui32_Remove(std::string& rstr_Dest,
                                   const std::string& krstr_Src,
                                   const std::string& krstr_SubStr,
                                   uint32_t ui32_StartPos) {
    uint32_t ui32_FoundPos = krstr_Src.find_first_of(krstr_SubStr, ui32_StartPos);
    if (std::string::npos != ui32_FoundPos) {
        std::string str;
        str.assign(krstr_Src.begin(), krstr_Src.begin() + ui32_FoundPos);
        str.append(krstr_Src.begin() + ui32_FoundPos + krstr_SubStr.size(), krstr_Src.end());
        rstr_Dest.assign(str);
    }
    return ui32_FoundPos;
}

std::string CStringHelper::fstr_RemoveAll(const std::string& krstr_Src,
                                          const std::string& krstr_SubStr) {
    std::string str_Result{krstr_Src};
    uint32_t ui32_FoundPos = 0;
    while(std::string::npos != ui32_FoundPos) {
        ui32_FoundPos = fui32_Remove(str_Result, str_Result, krstr_SubStr, ui32_FoundPos);
    }
    return str_Result;
}

uint32_t CStringHelper::fui32_Repace(std::string& rstr_Dest,
                                     const std::string& krstr_Src,
                                     const std::string& krstr_SubStr,
                                     const std::string& krstr_Replacement,
                                     uint32_t ui32_StartPos) {
    uint32_t ui32_Pos = krstr_Src.find_first_of(krstr_SubStr, ui32_StartPos);
    if (std::string::npos != ui32_Pos) {
        std::string str;
        str.assign(krstr_Src.begin(), krstr_Src.begin() + ui32_Pos);
        str.append(krstr_Replacement);
        str.append(krstr_Src.begin() + ui32_Pos + krstr_SubStr.size(), krstr_Src.end());
        rstr_Dest.assign(str);
    }
    return ui32_Pos;
}

std::string CStringHelper::fstr_RepaceAll(const std::string& krstr_Src,
                                          const std::string& krstr_SubStr,
                                          const std::string& krstr_Replacement) {
    std::string str_Result = krstr_Src;
    uint32_t ui32_FoundPos = 0U;
    while(true) {
        ui32_FoundPos = fui32_Repace(str_Result,
                                     str_Result,
                                     krstr_SubStr,
                                     krstr_Replacement,
                                     ui32_FoundPos);
        if (std::string::npos == ui32_FoundPos) {
            break;
        }
        // Avoid a part of the replacement and a part of behind origin create
        // a same thing as replaced sub-string
        ui32_FoundPos += krstr_Replacement.size();
    }
    return str_Result;
}

void CStringHelper::fv_LeftTrim(std::string& rstr, char c) {
    std::string::iterator iter_Last = std::find_if(rstr.begin(),
                                                   rstr.end(),
                                                   [c](char c_){return (c_ != c);});
    if (rstr.end() != iter_Last) {
        rstr.erase(rstr.begin(), iter_Last);
    }
}

void CStringHelper::fv_RightTrim(std::string& rstr, char c) {
    std::string::reverse_iterator riter_End = std::find_if(rstr.rbegin(),
                                                           rstr.rend(),
                                                           [c](char c_){return (c_ != c);});
    if (rstr.rend() != riter_End) {
        rstr.erase(rstr.end() - (riter_End - rstr.rbegin()), rstr.end());
    }
}

void CStringHelper::fv_Trim(std::string& rstr, char c) {
    fv_LeftTrim(rstr, c);
    fv_RightTrim(rstr, c);
}

std::string CStringHelper::fstr_Format(const std::string& kstr_Format, ...) {
    std::string str;
    va_list args;
    va_start(args, kstr_Format);
    // Question: Why I don't reuse fv_Format(kstr_Format, args)?
    // Answer: Because you will pass a __va_list_tag argument instead of your actual ones
    const uint32_t ui32_Size = vsnprintf(nullptr, 0, kstr_Format.c_str(), args) + 1;
    va_end(args);
    if (0 < ui32_Size) {
        char ac[ui32_Size];
        ac[ui32_Size - 1] = CHAR_ENDSTR;
        va_start(args, kstr_Format);
        vsnprintf(ac, ui32_Size, kstr_Format.c_str(), args);
        va_end(args);
        str.assign(ac);
    }
    return str;
}

void CStringHelper::fv_Format(std::string& rstr_Dest, const std::string& kstr_Format, ...) {
    va_list args;
    va_start(args, kstr_Format);
    const uint32_t ui32_Size = vsnprintf(nullptr, 0, kstr_Format.c_str(), args) + 1;
    va_end(args);
    if (0 < ui32_Size) {
        char ac[ui32_Size];
        ac[ui32_Size - 1] = CHAR_ENDSTR;
        va_start(args, kstr_Format);
        vsnprintf(ac, ui32_Size, kstr_Format.c_str(), args);
        va_end(args);
        rstr_Dest.assign(ac);
    }
}