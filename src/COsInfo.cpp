#include "COsInfo.hpp"
#include "utils/Definitions.hpp"
#include "helpers/FileReader.hpp"
#include "helpers/StringHandler.hpp"

#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <array>
#include <cstdlib>

namespace {

constexpr std::array<const char*, 2> ska_OsReleasePaths = {
    "/etc/os-release",
    "/usr/lib/os-release",
};

constexpr std::string_view ksv_KeyPrettyName = "PRETTY_NAME=";
constexpr std::string_view ksv_KeyName       = "NAME=";
constexpr std::string_view ksv_KeyId         = "ID=";

constexpr const char* kpc_DefaultOsName = "Linux";
constexpr const char* kpc_EnvShell      = "SHELL";
constexpr char        kc_PathSep        = '/';
constexpr char        kc_DoubleQuote    = '"';
constexpr char        kc_SingleQuote    = '\'';

std::string sfstr_UnquoteValue(std::string_view sv_Val) {
    std::string str_Val(sv_Val);
    StringHandler::fv_Trim(str_Val, Chars::ksv_Whitespace);
    if (2U <= str_Val.size() &&
        ((kc_DoubleQuote == str_Val.front() && kc_DoubleQuote == str_Val.back()) ||
         (kc_SingleQuote == str_Val.front() && kc_SingleQuote == str_Val.back()))) {
        str_Val = str_Val.substr(1U, str_Val.size() - 2U);
    }
    return str_Val;
}

std::string sfstr_BasenameOf(std::string_view sv_Path) {
    const std::size_t ui64_Slash = sv_Path.rfind(kc_PathSep);
    if (std::string_view::npos != ui64_Slash) {
        sv_Path.remove_prefix(ui64_Slash + 1U);
    }
    return std::string(sv_Path);
}

} // namespace

/**
 * @brief Construct COsInfo and eagerly load OS, kernel, shell, and uptime details.
 */
COsInfo::COsInfo() {
    fv_ReadOsReleaseFs();
    fv_ReadKernel();
    fv_ReadShellFs();
    fv_ReadUptime();
}

/**
 * @brief Read /etc/os-release (or /usr/lib/os-release) for OS name and ID.
 */
void COsInfo::fv_ReadOsReleaseFs() {
    std::vector<std::string> vecstr_Lines;
    bool b_Loaded = false;
    for (const char* kpc_Path : ska_OsReleasePaths) {
        if (FileReader::fb_ReadLines(vecstr_Lines, kpc_Path)) {
            b_Loaded = true;
            break;
        }
    }
    if (!b_Loaded) {
        mstr_Name = kpc_DefaultOsName;
        return;
    }

    std::string str_FallbackName;
    for (const std::string& krstr_Line : vecstr_Lines) {
        const std::string_view sv_Line(krstr_Line);
        if (sv_Line.starts_with(ksv_KeyPrettyName)) {
            mstr_Name = sfstr_UnquoteValue(sv_Line.substr(ksv_KeyPrettyName.size()));
        } else if (sv_Line.starts_with(ksv_KeyName)) {
            str_FallbackName = sfstr_UnquoteValue(sv_Line.substr(ksv_KeyName.size()));
        } else if (sv_Line.starts_with(ksv_KeyId)) {
            mstr_Id = sfstr_UnquoteValue(sv_Line.substr(ksv_KeyId.size()));
        }
    }

    if (mstr_Name.empty()) {
        mstr_Name = str_FallbackName.empty() ? kpc_DefaultOsName : std::move(str_FallbackName);
    }
}

/**
 * @brief Query uname(2) to retrieve the kernel release string.
 */
void COsInfo::fv_ReadKernel() {
    struct utsname uts_Info{};
    if (0 == ::uname(&uts_Info)) {
        mstr_Kernel.assign(uts_Info.release);
    }
}

/**
 * @brief Detect the active shell from $SHELL or parent process comm.
 */
void COsInfo::fv_ReadShellFs() {
    if (const char* kpc_EnvShellVal = std::getenv(kpc_EnvShell);
        nullptr != kpc_EnvShellVal && '\0' != kpc_EnvShellVal[0]) {
        mstr_Shell = sfstr_BasenameOf(kpc_EnvShellVal);
        if (!mstr_Shell.empty()) {
            return;
        }
    }

    const std::string str_CommPath = StringHandler::fstr_Format(Paths::kpc_ProcCommFmt, ::getppid());
    if (FileReader::fb_ReadFirstLine(mstr_Shell, str_CommPath)) {
        StringHandler::fv_Trim(mstr_Shell);
    }
}

/**
 * @brief Query sysinfo(2) to retrieve system uptime in seconds.
 */
void COsInfo::fv_ReadUptime() {
    struct sysinfo sys_Info{};
    if (0 == ::sysinfo(&sys_Info) && 0 <= sys_Info.uptime) {
        mui64_Uptime = static_cast<uint64_t>(sys_Info.uptime);
    }
}
