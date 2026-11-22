#ifndef CPPNEOFETCH_COLORS_HPP
#define CPPNEOFETCH_COLORS_HPP

namespace Colors {

inline constexpr const char* kpc_Normal      = "\033[0m";
inline constexpr const char* kpc_Black       = "\033[1;30m";
inline constexpr const char* kpc_Red         = "\033[1;31m";
inline constexpr const char* kpc_Green       = "\033[1;32m";
inline constexpr const char* kpc_Brown       = "\033[1;33m";
inline constexpr const char* kpc_Blue        = "\033[1;34m";
inline constexpr const char* kpc_Purple      = "\033[1;35m";
inline constexpr const char* kpc_Cyan        = "\033[1;36m";
inline constexpr const char* kpc_LightGray   = "\033[1;37m";
inline constexpr const char* kpc_DarkGray    = "\033[1;100m";
inline constexpr const char* kpc_LightRed    = "\033[1;101m";
inline constexpr const char* kpc_LightGreen  = "\033[1;102m";
inline constexpr const char* kpc_Yellow      = "\033[1;103m";
inline constexpr const char* kpc_LightBlue   = "\033[1;104m";
inline constexpr const char* kpc_LightPurple = "\033[1;105m";
inline constexpr const char* kpc_Teal        = "\033[1;106m";
inline constexpr const char* kpc_White       = "\033[1;107m";

inline constexpr int         ki32_BrightBgStart = 8;
inline constexpr int         ki32_BrightBgEnd   = 16;
inline constexpr const char* kpc_Bg256Fmt       = "\033[48;5;%dm   ";

} // namespace Colors

#endif // CPPNEOFETCH_COLORS_HPP
