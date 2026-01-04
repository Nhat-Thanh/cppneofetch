#include "CPackageManInfo.hpp"
#include "CStringHelper.hpp"
#include "CFileReader.hpp"
#include "Definitions.hpp"
#include <cstring>
#include <unistd.h>
#include <dirent.h>

#define PKGMAN_UNKNOWN "unknown"
// Debian/Ubuntu
#define PKGMAN_APT "apt"
#define PKGMAN_DPKG "dpkg"
// Arch Linux
#define PKGMAN_PACMAN "pacman"
// RHEL/CentOS/Fedora
#define PKGMAN_DNF "dnf"
#define PKGMAN_YUM "yum"
#define PKGMAN_RPM "rpm"
// Some others
#define PKGMAN_APK "apk"       // Alpine
#define PKGMAN_ZYPPER "zypper" // OpenSUSE
#define PKGMAN_EMERGE "emerge" // Gentoo

static uint32_t sfui32_CountDir(const char* kpc_Dir) {
    uint32_t ui32_Count = 0U;
    DIR* pDir = opendir(kpc_Dir);
    if (NULL != pDir) {
        struct dirent* entry = readdir(pDir);
        while (NULL != entry) {
            ui32_Count += static_cast<uint32_t>(DT_DIR == entry->d_type);
            entry = readdir(pDir);
        }
        // remove counts of .. and . entries
        if (2 <= ui32_Count) {
            ui32_Count -= 2;
        }
    }
    closedir(pDir);
    return ui32_Count;
}

static uint32_t sfui32_CountUnknownPkgs() {
    return 0U;
}

static uint32_t sfui32_CountDpkgPkgs() {
    uint32_t ui32_Count = 0U;
    CFileReader::fi64_CountLine(ui32_Count, "/var/lib/dpkg/status", "Package: ");
    return ui32_Count;
}

static uint32_t sfui32_CountPacmanPkgs() {
    return sfui32_CountDir("/var/lib/pacman/local");
}

static uint32_t sfui32_CountRpmPkgs() {
    uint32_t ui32_Count = 0U;
    FILE* pf = popen("rpm -qa | wc -l", "r");
    if (NULL != pf) {
        fscanf(pf, "%d", &ui32_Count);
    }
    pclose(pf);
    return ui32_Count;
}

static uint32_t sfui32_CountApkPkgs() {
    uint32_t ui32_Count = 0U;
    CFileReader::fi64_CountLine(ui32_Count, "/lib/apk/db/installed", "P:");
    return ui32_Count;
}

static uint32_t sfui32_CountEmergePkgs() {
    uint32_t ui32_Count = 0U;
    std::string str_Dir = "/var/db/pkg";
    std::string str_SubDir;
    DIR* pDir = opendir(str_Dir.c_str());
    if (NULL != pDir) {
        struct dirent* entry = readdir(pDir);
        while (NULL != entry) {
            if ((0 == strcmp(entry->d_name, ".")) || (0 == strcmp(entry->d_name, ".."))) {
                continue;
            }
            CStringHelper::fv_Format(str_SubDir, "%s/%s", str_Dir.c_str(), entry->d_name);
            ui32_Count += sfui32_CountDir(str_SubDir.c_str());
            entry = readdir(pDir);
        }
    }
    closedir(pDir);
    return ui32_Count;
}

CPackageManInfo::CPackageManInfo() {
    mpfui32_PkgCounter = nullptr;
}

std::string CPackageManInfo::fstr_GetName() const {
    if (mstr_Name.empty()) {
        std::vector<const char*> veckpc_KnownPkgManagers = {
            PKGMAN_APT,
            PKGMAN_DPKG,
            PKGMAN_PACMAN,
            PKGMAN_DNF,
            PKGMAN_YUM,
            PKGMAN_RPM,
            PKGMAN_APK,
            PKGMAN_ZYPPER,
            PKGMAN_EMERGE
        };
        std::string str_Path;
        for (const char* kpc_PkgMan : veckpc_KnownPkgManagers) {
            CStringHelper::fv_Format(str_Path, "/usr/bin/%s", kpc_PkgMan);
            if (SYSCALL_SUCCESS == access(str_Path.c_str(), F_OK)) {
                mstr_Name.assign(kpc_PkgMan);
                break;
            }
        }
        if (mstr_Name.empty()) {
            mstr_Name.assign(PKGMAN_UNKNOWN);
        }
    }
    return mstr_Name;
}

uint32_t CPackageManInfo::fui32_CountPkgs() const {
    // Must determine the package manager first
    fstr_GetName();
    if (nullptr == mpfui32_PkgCounter) {
        typedef struct {
            const char* kpc_PkgMan;
            uint32_t (*pfui32_Counter)();
        } pkg_counter_t;

        std::vector<pkg_counter_t> vec_KnownPkgCounters = {
            { PKGMAN_APT,    sfui32_CountDpkgPkgs   },
            { PKGMAN_DPKG,   sfui32_CountDpkgPkgs   },
            { PKGMAN_PACMAN, sfui32_CountPacmanPkgs },
            { PKGMAN_DNF,    sfui32_CountRpmPkgs    },
            { PKGMAN_YUM,    sfui32_CountRpmPkgs    },
            { PKGMAN_RPM,    sfui32_CountRpmPkgs    },
            { PKGMAN_APK,    sfui32_CountApkPkgs    },
            { PKGMAN_ZYPPER, sfui32_CountRpmPkgs    },
            { PKGMAN_EMERGE, sfui32_CountEmergePkgs }
        };
        for (const pkg_counter_t& PkgCounter : vec_KnownPkgCounters) {
            if (0 == mstr_Name.compare(PkgCounter.kpc_PkgMan)) {
                mpfui32_PkgCounter = PkgCounter.pfui32_Counter;
            }
        }
        if (nullptr == mpfui32_PkgCounter) {
            mpfui32_PkgCounter = sfui32_CountUnknownPkgs;
        }
    }

    return mpfui32_PkgCounter();
}
