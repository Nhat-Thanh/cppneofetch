#include "CCpuInfo.hpp"
#include "CMemInfo.hpp"
#include "CProductInfo.hpp"
#include "CTerminalInfo.hpp"
#include "COsInfo.hpp"
#include "CPackageManInfo.hpp"
#include "CHostInfo.hpp"
#include "CStringHelper.hpp"
#include "Colors.hpp"
#include "Logo.hpp"
#include <unistd.h>
#include <iostream>

static std::string sfstr_FormatHost(CHostInfo&& host) {
    return CStringHelper::fstr_Format(CYAN "%s" NORMAL "@" CYAN "%s" NORMAL,
                                      host.fstr_GetUser().c_str(),
                                      host.fstr_GetName().c_str());
}

static std::string sfstr_FormatPkgMan(CPackageManInfo&& pkgman) {
    return CStringHelper::fstr_Format("%d (%s)",
                                      pkgman.fui32_CountPkgs(),
                                      pkgman.fstr_GetName().c_str());
}

static std::string sfstr_FormatCpu(CCpuInfo&& cpu) {
    return CStringHelper::fstr_Format("%s (%d) @ %.2fGHz",
                                      cpu.fstr_GetName().c_str(),
                                      cpu.fui32_GetLogicalCores(),
                                      cpu.fui32_GetMaxFreq() / 1000000.0);
}

static std::string sfstr_FormatMem(CMemInfo&& mem) {
    return CStringHelper::fstr_Format("%dMiB / %dMiB (%.2f%)",
                                      mem.fui32_GetUsed() / 1024U,
                                      mem.fui32_GetTotal() / 1024U,
                                      100.0f * mem.fui32_GetUsed() / mem.fui32_GetTotal());
}

static std::string sfstr_FormatUptime(uint64_t ui64_Uptimes) {
    std::string str_Uptime;
    typedef struct {
        const char* kpc_Unit;
        uint32_t ui32_InSeconds;
    } unit_in_sec_t;
    std::vector<unit_in_sec_t> vec_Units = {
        { "day",  60 * 60 * 24 },
        { "hour", 60 * 60      },
        { "min",  60           }
    };
    for (const unit_in_sec_t& unit : vec_Units) {
        uint64_t ui64_time = ui64_Uptimes / unit.ui32_InSeconds;
        if (0 != ui64_time) {
            std::string str;
            if (1 == ui64_time) {
                CStringHelper::fv_Format(str, "%d %s ", ui64_time, unit.kpc_Unit);
            } else {
                CStringHelper::fv_Format(str, "%d %ss ", ui64_time, unit.kpc_Unit);
            }
            str_Uptime.append(str);
        }
        ui64_Uptimes %= unit.ui32_InSeconds;
    }
    return str_Uptime;
}

static std::string sfstr_GetColorBar() {
    std::string str_ColorBar;
    std::string str_Color;
    for (int i32_ColorNumber = 8; i32_ColorNumber < 16; ++i32_ColorNumber) {
        CStringHelper::fv_Format(str_Color, "\e[48;5;%dm   ", i32_ColorNumber);
        str_ColorBar.append(str_Color);
    }
    CStringHelper::fv_Format(str_ColorBar, "%s%s", str_ColorBar.c_str(), NORMAL);
    return str_ColorBar;
}

int main() {
    CCpuInfo cpu;
    CMemInfo mem;
    CProductInfo product;
    CTerminalInfo terminal;
    COsInfo os;
    CPackageManInfo pkgman;
    CHostInfo host;

    std::vector<std::string> vecstr_Fields = {
        "",     // user@host
        "",     // ---------
        "OS: ",
        "Model: ",
        "Vendor: ",
        "Kernel: ",
        "Uptime: ",
        "",     // emtpy line
        "Packages: ",
        "Shell: ",
        "Terminal: ",
        "",     // emtpy line
        "CPU: ",
        "Memnory: ",
        "",     // emtpy line
        ""      // color bar
    };

    std::vector<std::string> vecstr_Infos = {
        sfstr_FormatHost(std::move(host)),
        std::string(host.fstr_GetUser().size() + host.fstr_GetName().size() + 1, '-'),
        os.fstr_GetName(),
        product.fstr_GetName(),
        product.fstr_GetVendor(),
        os.fstr_GetKernel(),
        sfstr_FormatUptime(os.fui64_GetUptime()),
        "",
        sfstr_FormatPkgMan(std::move(pkgman)),
        os.fstr_GetShell(),
        terminal.fstr_GetName(),
        "",
        sfstr_FormatCpu(std::move(cpu)),
        sfstr_FormatMem(std::move(mem)),
        "",
        sfstr_GetColorBar()};

    if (vecstr_Fields.size() != vecstr_Infos.size()) {
        std::cerr << "Number of fields are different from number of infos" << std::endl;
        return EXIT_FAILURE;
    }
    size_t ui64_LogoRow = 0UL, ui64_InfoRow = 0UL;

    while (ui64_InfoRow < vecstr_Infos.size() || ui64_LogoRow < vecstr_Logo.size()) {
        if (ui64_InfoRow == vecstr_Infos.size()) {
            std::cout << CYAN << vecstr_Logo[ui64_LogoRow] << std::endl;
            ++ui64_LogoRow;

        } else if (ui64_LogoRow == vecstr_Logo.size()) {
            std::cout << NORMAL << std::string(vecstr_Logo.front().size(), ' ');
            std::cout << CYAN << vecstr_Fields[ui64_InfoRow];
            std::cout << NORMAL << vecstr_Infos[ui64_InfoRow] << std::endl;
            ++ui64_InfoRow;

        } else {
            std::cout << CYAN << vecstr_Logo[ui64_LogoRow];
            std::cout << CYAN << vecstr_Fields[ui64_InfoRow];
            std::cout << NORMAL << vecstr_Infos[ui64_InfoRow] << std::endl;
            ++ui64_LogoRow;
            ++ui64_InfoRow;
        }
    }

    return 0;
}