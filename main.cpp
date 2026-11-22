#include "CCpuInfo.hpp"
#include "CGpuInfo.hpp"
#include "CHostInfo.hpp"
#include "CMemInfo.hpp"
#include "COsInfo.hpp"
#include "CPackageManInfo.hpp"
#include "CProductInfo.hpp"
#include "CTerminalInfo.hpp"
#include "utils/Colors.hpp"
#include "utils/Definitions.hpp"
#include "utils/Logo.hpp"
#include "helpers/StringHandler.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <utility>

namespace {

namespace Labels {

constexpr const char* kpc_Os         = "OS: ";
constexpr const char* kpc_Model      = "Model: ";
constexpr const char* kpc_Vendor     = "Vendor: ";
constexpr const char* kpc_Kernel     = "Kernel: ";
constexpr const char* kpc_Uptime     = "Uptime: ";
constexpr const char* kpc_Packages   = "Packages: ";
constexpr const char* kpc_Shell      = "Shell: ";
constexpr const char* kpc_Terminal   = "Terminal: ";
constexpr const char* kpc_Cpu        = "CPU: ";
constexpr const char* kpc_GpuSingle  = "GPU: ";
constexpr const char* kpc_GpuMulti   = "GPU %zu: ";
constexpr const char* kpc_Memory     = "Memory: ";
constexpr const char* kpc_UnknownCpu = "Unknown CPU";
constexpr const char* kpc_UnknownPkg = "0 (unknown)";

} // namespace Labels

namespace Formats {

constexpr const char* kpc_Host          = "%s%s%s@%s%s%s";
constexpr const char* kpc_PkgEntry      = "%u (%s)";
constexpr const char* kpc_PkgSep        = ", ";
constexpr const char* kpc_CpuWithFreq   = "%s (%u) @ %.2fGHz";
constexpr const char* kpc_CpuNoFreq     = "%s (%u)";
constexpr const char* kpc_GpuFreqSuffix = " @ %.2fGHz";
constexpr const char* kpc_Memory        = "%lluMiB / %lluMiB (%.2f%%)";
constexpr const char* kpc_UptimeUnit    = "%llu %s%s";
constexpr const char* kpc_UptimeSec     = "%llu sec%s";
constexpr const char* kpc_PluralSuffix  = "s";
constexpr const char* kpc_Empty         = "";
constexpr char        kc_SepChar        = '-';
constexpr char        kc_PadChar        = ' ';

} // namespace Formats

typedef struct {
    std::string str_Label;
    std::string str_Value;
} info_row_t;

std::string fstr_FormatHost(const CHostInfo& kr_Host) {
    return StringHandler::fstr_Format(Formats::kpc_Host,
                                      Colors::kpc_Cyan, kr_Host.fstr_GetUser().c_str(), Colors::kpc_Normal,
                                      Colors::kpc_Cyan, kr_Host.fstr_GetName().c_str(), Colors::kpc_Normal);
}

std::string fstr_FormatPkgMan(const CPackageManInfo& kr_PkgMan) {
    const auto& krvec_Managers = kr_PkgMan.fkrvec_GetManagers();
    if (krvec_Managers.empty()) {
        return Labels::kpc_UnknownPkg;
    }
    std::string str_Out;
    for (std::size_t ui64_Idx = 0U; ui64_Idx < krvec_Managers.size(); ++ui64_Idx) {
        if (0U < ui64_Idx) {
            str_Out.append(Formats::kpc_PkgSep);
        }
        str_Out.append(StringHandler::fstr_Format(Formats::kpc_PkgEntry,
                                                  krvec_Managers[ui64_Idx].ui32_Count,
                                                  krvec_Managers[ui64_Idx].str_Name.c_str()));
    }
    return str_Out;
}

std::string fstr_FormatCpu(const CCpuInfo& kr_Cpu) {
    const std::string& krstr_Name =
        kr_Cpu.fstr_GetName().empty() ? std::string(Labels::kpc_UnknownCpu) : kr_Cpu.fstr_GetName();
    if (const auto optui64_FreqKhz = kr_Cpu.foptui64_GetMaxFreqKhz()) {
        return StringHandler::fstr_Format(Formats::kpc_CpuWithFreq,
                                          krstr_Name.c_str(),
                                          kr_Cpu.fui32_GetLogicalCores(),
                                          static_cast<double>(*optui64_FreqKhz) / Units::kd_KhzPerGhz);
    }
    return StringHandler::fstr_Format(Formats::kpc_CpuNoFreq,
                                      krstr_Name.c_str(),
                                      kr_Cpu.fui32_GetLogicalCores());
}

std::string fstr_FormatMem(const CMemInfo& kr_Mem) {
    const uint64_t ui64_TotalMib = kr_Mem.fui64_GetTotal() / Units::kui64_KibPerMib;
    const uint64_t ui64_UsedMib = kr_Mem.fui64_GetUsed() / Units::kui64_KibPerMib;
    const double d_Percent = (static_cast<double>(kr_Mem.fui64_GetUsed()) / 
                              static_cast<double>(kr_Mem.fui64_GetTotal()) * 
                              Units::kd_PercentScale);
    return StringHandler::fstr_Format(Formats::kpc_Memory,
                                      static_cast<unsigned long long>(ui64_UsedMib),
                                      static_cast<unsigned long long>(ui64_TotalMib),
                                      d_Percent);
}

std::string fstr_FormatUptime(uint64_t ui64_TotalSeconds) {
    typedef struct {
        const char* kpc_Unit;
        uint64_t ui64_InSeconds;
    } unit_in_sec_t;

    constexpr unit_in_sec_t ka_Units[] = {
        { "day",  Units::kui64_SecPerDay  },
        { "hour", Units::kui64_SecPerHour },
        { "min",  Units::kui64_SecPerMin  },
    };

    std::string str_Uptime;
    uint64_t ui64_Rem = ui64_TotalSeconds;
    for (const unit_in_sec_t& kr_Unit : ka_Units) {
        const uint64_t ui64_Val = ui64_Rem / kr_Unit.ui64_InSeconds;
        if (0ULL < ui64_Val) {
            if (!str_Uptime.empty()) {
                str_Uptime.push_back(Formats::kc_PadChar);
            }
            str_Uptime.append(StringHandler::fstr_Format(
                Formats::kpc_UptimeUnit,
                static_cast<unsigned long long>(ui64_Val),
                kr_Unit.kpc_Unit,
                (1ULL == ui64_Val) ? Formats::kpc_Empty : Formats::kpc_PluralSuffix));
        }
        ui64_Rem %= kr_Unit.ui64_InSeconds;
    }

    if (str_Uptime.empty()) {
        str_Uptime = StringHandler::fstr_Format(
            Formats::kpc_UptimeSec,
            static_cast<unsigned long long>(ui64_Rem),
            (1ULL == ui64_Rem) ? Formats::kpc_Empty : Formats::kpc_PluralSuffix);
    }
    return str_Uptime;
}

std::string fstr_GetColorBar() {
    std::string str_ColorBar;
    for (int i32_ColorNumber = Colors::ki32_BrightBgStart;
         i32_ColorNumber < Colors::ki32_BrightBgEnd;
         ++i32_ColorNumber) {
        str_ColorBar.append(StringHandler::fstr_Format(Colors::kpc_Bg256Fmt, i32_ColorNumber));
    }
    str_ColorBar.append(Colors::kpc_Normal);
    return str_ColorBar;
}

} // namespace

int main() {
    const CHostInfo host;
    const COsInfo os;
    const CProductInfo product;
    const CPackageManInfo pkgman;
    const CTerminalInfo terminal;
    const CCpuInfo cpu;
    const CGpuInfo gpu;
    const CMemInfo mem;

    std::vector<info_row_t> vec_Rows;
    vec_Rows.push_back({ Formats::kpc_Empty, fstr_FormatHost(host) });

    const std::size_t ui64_SepLen = host.fstr_GetUser().size() + host.fstr_GetName().size() + 1U;
    vec_Rows.push_back({ Formats::kpc_Empty, std::string(ui64_SepLen, Formats::kc_SepChar) });

    auto fn_AddIfNotEmpty = [&](const char* kpc_Label, const std::string& krstr_Val) {
        if (!krstr_Val.empty()) {
            vec_Rows.push_back({ kpc_Label, krstr_Val });
        }
    };

    fn_AddIfNotEmpty(Labels::kpc_Os, os.fstr_GetName());
    fn_AddIfNotEmpty(Labels::kpc_Model, product.fstr_GetName());
    fn_AddIfNotEmpty(Labels::kpc_Vendor, product.fstr_GetVendor());
    fn_AddIfNotEmpty(Labels::kpc_Kernel, os.fstr_GetKernel());
    vec_Rows.push_back({ Labels::kpc_Uptime, fstr_FormatUptime(os.fui64_GetUptime()) });
    vec_Rows.push_back({ Labels::kpc_Packages, fstr_FormatPkgMan(pkgman) });
    fn_AddIfNotEmpty(Labels::kpc_Shell, os.fstr_GetShell());
    fn_AddIfNotEmpty(Labels::kpc_Terminal, terminal.fstr_GetName());
    vec_Rows.push_back({ Labels::kpc_Cpu, fstr_FormatCpu(cpu) });

    const auto& krvec_Gpus = gpu.fkrvec_GetDevices();
    for (std::size_t ui64_Idx = 0U; ui64_Idx < krvec_Gpus.size(); ++ui64_Idx) {
        const std::string str_Label = (1U == krvec_Gpus.size()) 
                                      ? Labels::kpc_GpuSingle 
                                      : StringHandler::fstr_Format(Labels::kpc_GpuMulti, ui64_Idx + 1U);
        std::string str_Value = krvec_Gpus[ui64_Idx].str_Name;
        if (krvec_Gpus[ui64_Idx].optui32_MaxFreqMhz) {
            str_Value.append(StringHandler::fstr_Format(
                Formats::kpc_GpuFreqSuffix,
                static_cast<double>(*krvec_Gpus[ui64_Idx].optui32_MaxFreqMhz) / Units::kd_MhzPerGhz));
        }
        vec_Rows.push_back({ str_Label, std::move(str_Value) });
    }

    vec_Rows.push_back({ Labels::kpc_Memory, fstr_FormatMem(mem) });
    vec_Rows.push_back({ Formats::kpc_Empty, Formats::kpc_Empty });
    vec_Rows.push_back({ Formats::kpc_Empty, fstr_GetColorBar() });

    constexpr std::size_t kui64_LogoRows = std::size(ka_Logo);
    const std::size_t ui64_LogoWidth = kui64_LogoRows == 0U
        ? 0U
        : StringHandler::fui64_Utf8DisplayWidth(ka_Logo[0]);
    const std::string str_LogoPad(ui64_LogoWidth, Formats::kc_PadChar);

    const std::size_t ui64_TotalLines = std::max(kui64_LogoRows, vec_Rows.size());
    for (std::size_t ui64_Idx = 0U; ui64_Idx < ui64_TotalLines; ++ui64_Idx) {
        if (kui64_LogoRows > ui64_Idx) {
            std::cout << Colors::kpc_Cyan << ka_Logo[ui64_Idx];
        } else {
            std::cout << Colors::kpc_Normal << str_LogoPad;
        }

        if (vec_Rows.size() > ui64_Idx) {
            std::cout << Colors::kpc_Cyan << vec_Rows[ui64_Idx].str_Label
                      << Colors::kpc_Normal << vec_Rows[ui64_Idx].str_Value;
        }
        std::cout << '\n';
    }
    std::cout << Colors::kpc_Normal << std::flush;

    return 0;
}
