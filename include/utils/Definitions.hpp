#ifndef CPPNEOFETCH_DEFINITIONS_HPP
#define CPPNEOFETCH_DEFINITIONS_HPP

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace Units {

inline constexpr uint64_t kui64_KibPerMib   = 1024ULL;
inline constexpr uint64_t kui64_HzPerMhz    = 1000000ULL;
inline constexpr uint64_t kui64_KhzPerMhz   = 1000ULL;
inline constexpr double   kd_KhzPerGhz      = 1000000.0;
inline constexpr double   kd_MhzPerGhz      = 1000.0;
inline constexpr double   kd_PercentScale   = 100.0;

inline constexpr uint64_t kui64_SecPerMin   = 60ULL;
inline constexpr uint64_t kui64_SecPerHour  = 60ULL * kui64_SecPerMin;
inline constexpr uint64_t kui64_SecPerDay   = 24ULL * kui64_SecPerHour;

} // namespace Units

namespace NumericBase {

inline constexpr int ki32_Decimal = 10;
inline constexpr int ki32_Hex     = 16;

} // namespace NumericBase

namespace BufferSizes {

inline constexpr std::size_t kui64_LineBuf    = 4096U;
inline constexpr std::size_t kui64_ReadBuf    = 8192U;
inline constexpr std::size_t kui64_PwEntryBuf = 4096U;

} // namespace BufferSizes

namespace Paths {

inline constexpr const char* kpc_ProcStatFmt    = "/proc/%d/stat";
inline constexpr const char* kpc_ProcCommFmt    = "/proc/%d/comm";
inline constexpr const char* kpc_ProcCmdlineFmt = "/proc/%d/cmdline";
inline constexpr const char* kpc_SysPciDevBus   = "/sys/bus/pci/devices";

} // namespace Paths

namespace Pci {

inline constexpr uint32_t kui32_ClassDisplayCtrl = 0x00000003U;
inline constexpr uint32_t kui32_ClassCodeShift   = 16U;

} // namespace Pci

namespace Chars {

inline constexpr std::string_view ksv_Whitespace = " \t\r\n";

} // namespace Chars

#endif // CPPNEOFETCH_DEFINITIONS_HPP
