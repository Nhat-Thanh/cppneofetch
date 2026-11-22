#include "CPackageManInfo.hpp"
#include "helpers/FileReader.hpp"
#include "helpers/RaiiHandles.hpp"

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <array>
#include <cstdlib>
#include <cstring>
#include <string_view>

namespace {

constexpr const char* kpc_DirSelf             = ".";
constexpr const char* kpc_DirParent           = "..";
constexpr const char* kpc_PathSep             = "/";

constexpr const char* kpc_PkgManUnknown       = "unknown";
constexpr const char* kpc_PkgManPacman        = "pacman";
constexpr const char* kpc_PkgManApt           = "apt";
constexpr const char* kpc_PkgManDpkg          = "dpkg";
constexpr const char* kpc_PkgManApk           = "apk";
constexpr const char* kpc_PkgManEmerge        = "emerge";
constexpr const char* kpc_PkgManRpm           = "rpm";
constexpr const char* kpc_PkgManFlatpak       = "flatpak";

constexpr const char* kpc_DbPacmanLocal       = "/var/lib/pacman/local";
constexpr const char* kpc_DbDpkgStatus        = "/var/lib/dpkg/status";
constexpr const char* kpc_DbApkInstalled      = "/lib/apk/db/installed";
constexpr const char* kpc_DbEmergePkg         = "/var/db/pkg";
constexpr const char* kpc_DbRpmLegacy         = "/var/lib/rpm";
constexpr const char* kpc_DbRpmSysimage       = "/usr/lib/sysimage/rpm";
constexpr const char* kpc_DbFlatpakSystem     = "/var/lib/flatpak/app";
constexpr const char* kpc_DbFlatpakUserSub    = "/.local/share/flatpak/app";
constexpr const char* kpc_BinApt              = "/usr/bin/apt";
constexpr const char* kpc_EnvHome             = "HOME";

constexpr std::string_view ksv_DpkgInstalled  = "Status: install ok installed";
constexpr std::string_view ksv_ApkPkgPrefix   = "P:";

constexpr const char* kpc_CmdRpmCount         = "rpm -qa 2>/dev/null | wc -l";
constexpr const char* kpc_PipeReadMode        = "r";
constexpr const char* kpc_RpmCountFmt         = "%u";

typedef struct {
    const char* kpc_BinPath;
    const char* kpc_Name;
} rpm_frontend_t;

constexpr std::array<rpm_frontend_t, 3> ska_RpmFrontends = {{
    { "/usr/bin/dnf",    "dnf"    },
    { "/usr/bin/zypper", "zypper" },
    { "/usr/bin/yum",    "yum"    },
}};

bool sfb_CountSubdirs(uint32_t& rui32_Count, const std::string& krstr_DirPath) {
    UniqueDir pDir(::opendir(krstr_DirPath.c_str()));
    if (!pDir) {
        return false;
    }

    uint32_t ui32_Count = 0U;
    struct dirent* p_Entry = nullptr;
    while ((p_Entry = ::readdir(pDir.get())) != nullptr) {
        if (0 == std::strcmp(p_Entry->d_name, kpc_DirSelf) ||
            0 == std::strcmp(p_Entry->d_name, kpc_DirParent)) {
            continue;
        }
        if (DT_DIR == p_Entry->d_type) {
            ++ui32_Count;
        } else if (DT_UNKNOWN == p_Entry->d_type) {
            struct stat st{};
            const std::string str_FullPath = krstr_DirPath + kpc_PathSep + p_Entry->d_name;
            if (0 == ::stat(str_FullPath.c_str(), &st) && S_ISDIR(st.st_mode)) {
                ++ui32_Count;
            }
        }
    }
    rui32_Count = ui32_Count;
    return true;
}

bool sfb_CountEmergePkgs(uint32_t& rui32_Count) {
    UniqueDir pDir(::opendir(kpc_DbEmergePkg));
    if (!pDir) {
        return false;
    }

    uint32_t ui32_Total = 0U;
    struct dirent* p_Entry = nullptr;
    while ((p_Entry = ::readdir(pDir.get())) != nullptr) {
        if (0 == std::strcmp(p_Entry->d_name, kpc_DirSelf) ||
            0 == std::strcmp(p_Entry->d_name, kpc_DirParent)) {
            continue;
        }
        uint32_t ui32_SubCount = 0U;
        if (sfb_CountSubdirs(ui32_SubCount, std::string(kpc_DbEmergePkg) + kpc_PathSep + p_Entry->d_name)) {
            ui32_Total += ui32_SubCount;
        }
    }
    rui32_Count = ui32_Total;
    return true;
}

bool sfb_CountRpmPkgs(uint32_t& rui32_Count) {
    if (0 != ::access(kpc_DbRpmLegacy, F_OK) &&
        0 != ::access(kpc_DbRpmSysimage, F_OK)) {
        return false;
    }
    UniquePipe pPipe(::popen(kpc_CmdRpmCount, kpc_PipeReadMode));
    if (!pPipe) {
        return false;
    }
    uint32_t ui32_Count = 0U;
    if (1 == std::fscanf(pPipe.get(), kpc_RpmCountFmt, &ui32_Count)) {
        rui32_Count = ui32_Count;
        return true;
    }
    return false;
}

} // namespace

/**
 * @brief Construct CPackageManInfo and detect installed package managers.
 */
CPackageManInfo::CPackageManInfo() {
    uint32_t ui32_Count = 0U;
    // TODO: Investigate that there're any multi-package-manager systems

    // Prefer DB-backed native counts; check pacman first (Arch Linux primary)
    if (sfb_CountSubdirs(ui32_Count, kpc_DbPacmanLocal) && 0U < ui32_Count) {
        mvec_Managers.push_back({kpc_PkgManPacman, ui32_Count});
    }

    if (FileReader::fb_CountLinesContaining(ui32_Count, kpc_DbDpkgStatus, ksv_DpkgInstalled) &&
        0U < ui32_Count) {
        const char* kpc_Name = (::access(kpc_BinApt, F_OK) == 0) ? kpc_PkgManApt : kpc_PkgManDpkg;
        mvec_Managers.push_back({kpc_Name, ui32_Count});
    }

    if (FileReader::fb_CountLinesContaining(ui32_Count, kpc_DbApkInstalled, ksv_ApkPkgPrefix) &&
        0U < ui32_Count) {
        mvec_Managers.push_back({kpc_PkgManApk, ui32_Count});
    }

    if (sfb_CountEmergePkgs(ui32_Count) && 0U < ui32_Count) {
        mvec_Managers.push_back({kpc_PkgManEmerge, ui32_Count});
    }

    if (sfb_CountRpmPkgs(ui32_Count) && 0U < ui32_Count) {
        const char* kpc_Name = kpc_PkgManRpm;
        for (const rpm_frontend_t& kr_Frontend : ska_RpmFrontends) {
            if (0 == ::access(kr_Frontend.kpc_BinPath, F_OK)) {
                kpc_Name = kr_Frontend.kpc_Name;
                break;
            }
        }
        mvec_Managers.push_back({kpc_Name, ui32_Count});
    }

    // Also include Flatpak apps if installed (system + user)
    uint32_t ui32_FlatpakCount = 0U;
    if (sfb_CountSubdirs(ui32_Count, kpc_DbFlatpakSystem)) {
        ui32_FlatpakCount += ui32_Count;
    }
    if (const char* kpc_Home = std::getenv(kpc_EnvHome);
        nullptr != kpc_Home && '\0' != kpc_Home[0]) {
        if (sfb_CountSubdirs(ui32_Count, std::string(kpc_Home) + kpc_DbFlatpakUserSub)) {
            ui32_FlatpakCount += ui32_Count;
        }
    }
    if (0U < ui32_FlatpakCount) {
        mvec_Managers.push_back({kpc_PkgManFlatpak, ui32_FlatpakCount});
    }
}

/**
 * @brief Get the primary package manager name (or "unknown" if none detected).
 *
 * @return Package manager name string
 */
std::string CPackageManInfo::fstr_GetName() const {
    if (mvec_Managers.empty()) {
        return kpc_PkgManUnknown;
    }
    return mvec_Managers.front().str_Name;
}

/**
 * @brief Get the sum of installed packages across all detected package managers.
 *
 * @return Total installed package count
 */
uint32_t CPackageManInfo::fui32_CountPkgs() const noexcept {
    uint32_t ui32_Total = 0U;
    for (const pkg_manager_t& kr_Entry : mvec_Managers) {
        ui32_Total += kr_Entry.ui32_Count;
    }
    return ui32_Total;
}
