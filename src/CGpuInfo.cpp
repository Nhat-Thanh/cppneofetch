#include "CGpuInfo.hpp"
#include "PciIds.hpp"
#include "utils/Definitions.hpp"
#include "helpers/FileReader.hpp"
#include "helpers/RaiiHandles.hpp"
#include "helpers/StringHandler.hpp"

#include <dirent.h>
#include <algorithm>
#include <array>
#include <charconv>
#include <fstream>

namespace {

constexpr std::array<const char*, 2> ska_SystemPciIdsPaths = {
    "/usr/share/hwdata/pci.ids",
    "/usr/share/misc/pci.ids",
};

constexpr const char* kpc_PciIdsVendorPrefixFmt  = "%04x  ";
constexpr const char* kpc_PciIdsDevicePrefixFmt  = "\t%04x  ";
constexpr const char* kpc_UnknownDeviceFmt       = "%s [Device %04x]";
constexpr const char* kpc_UnknownPciDisplayFmt   = "PCI Display Controller [%04x:%04x]";

constexpr std::string_view ksv_PciClassSectionPrefix = "C ";
constexpr std::string_view ksv_SysFileClass          = "/class";
constexpr std::string_view ksv_SysFileVendor         = "/vendor";
constexpr std::string_view ksv_SysFileDevice         = "/device";
constexpr std::string_view ksv_SysSubdirDrm          = "/drm";
constexpr std::string_view ksv_SysSubdirHwmon        = "/hwmon";
constexpr std::string_view ksv_SysFileAmdDpmSclk     = "/pp_dpm_sclk";
constexpr std::string_view ksv_SysFileHwmonMaxFreq   = "/freq1_max";
constexpr std::string_view ksv_CardPrefix            = "card";
constexpr std::string_view ksv_HwmonPrefix           = "hwmon";

constexpr std::array<std::string_view, 2> ska_IntelFreqFiles = {
    "/gt_max_freq_mhz",
    "/gt/gt0/rps_max_freq_mhz",
};

constexpr char kc_PciAddrSep       = ':';
constexpr char kc_CardConnSep      = '-';
constexpr char kc_DpmColonSep      = ':';
constexpr char kc_PciCommentPrefix = '#';
constexpr char kc_PciTabIndent     = '\t';

#if 0
// TODO: Keep this function as an althernative method if the pci.ids file cannot be downloaded to generate PciIds.hpp header
// TODO: remove in-body returning
bool sfb_LookupSystemPciIdsFile(uint16_t ui16_VendorId,
                                uint16_t ui16_DeviceId,
                                std::string& rstr_VendorName,
                                std::string& rstr_DeviceName) {
    const std::string str_VendorPrefix = StringHandler::fstr_Format(kpc_PciIdsVendorPrefixFmt, ui16_VendorId);
    const std::string str_DevicePrefix = StringHandler::fstr_Format(kpc_PciIdsDevicePrefixFmt, ui16_DeviceId);

    for (const char* kpc_Path : ska_SystemPciIdsPaths) {
        std::ifstream ifs(kpc_Path);
        if (!ifs.is_open()) {
            continue;
        }
        std::string str_Line;
        std::string str_Vendor;
        bool b_InTargetVendor = false;
        while (std::getline(ifs, str_Line)) {
            if (str_Line.empty() || kc_PciCommentPrefix == str_Line[0]) {
                continue;
            }
            if (str_Line.starts_with(ksv_PciClassSectionPrefix)) {
                break;
            }
            if (kc_PciTabIndent != str_Line[0]) {
                if (b_InTargetVendor) {
                    rstr_VendorName = std::move(str_Vendor);
                    rstr_DeviceName.clear();
                    return true;
                }
                if (str_Line.starts_with(str_VendorPrefix)) {
                    str_Vendor = str_Line.substr(str_VendorPrefix.size());
                    b_InTargetVendor = true;
                }
            } else if (b_InTargetVendor && 1U < str_Line.size() && kc_PciTabIndent != str_Line[1]) {
                if (str_Line.starts_with(str_DevicePrefix)) {
                    rstr_VendorName = std::move(str_Vendor);
                    rstr_DeviceName = str_Line.substr(str_DevicePrefix.size());
                    return true;
                }
            }
        }
        if (b_InTargetVendor) {
            rstr_VendorName = std::move(str_Vendor);
            rstr_DeviceName.clear();
            return true;
        }
    }
    return false;
}
#endif

std::string sfstr_FormatGpuName(const std::string_view& krsv_Vendor, const std::string_view& krsv_Device) {
    if (krsv_Device.empty()) {
        return std::string(krsv_Vendor);
    }
    // TODO: implement a fstr_GetSubstringBetween but return string_view type
    const std::string str_ShortDev = StringHandler::fstr_GetSubstringBetween(krsv_Device, '[', ']');
    const std::string_view sv_Chosen = str_ShortDev.empty() ? krsv_Device : std::string_view(str_ShortDev);
    if (krsv_Vendor.empty()) {
        return std::string(sv_Chosen);
    }
    return std::string(krsv_Vendor) + " " + std::string(sv_Chosen);
}

std::optional<uint32_t> sfopt_ReadAmdDpmMaxMhz(const std::string& krstr_Path) {
    std::vector<std::string> vecstr_Lines;
    if (!FileReader::fb_ReadLines(vecstr_Lines, krstr_Path)) {
        return std::nullopt;
    }
    uint32_t ui32_MaxMhz = 0U;
    for (const std::string& krstr_Line : vecstr_Lines) {
        // Format: "0: 300Mhz " or "1: 2400Mhz *"
        const std::size_t ui64_Colon = krstr_Line.find(kc_DpmColonSep);
        if (std::string::npos == ui64_Colon) {
            continue;
        }
        std::string_view sv_Val(krstr_Line);
        sv_Val.remove_prefix(ui64_Colon + 1U);
        while (!sv_Val.empty() && sv_Val.front() == ' ') {
            sv_Val.remove_prefix(1U);
        }
        uint32_t ui32_Mhz = 0U;
        const auto [p_Ptr, errCode] = std::from_chars(
            sv_Val.data(), sv_Val.data() + sv_Val.size(), ui32_Mhz, NumericBase::ki32_Decimal);
        if (std::errc{} == errCode && sv_Val.data() != p_Ptr && ui32_MaxMhz < ui32_Mhz) {
            ui32_MaxMhz = ui32_Mhz;
        }
    }
    return (0U < ui32_MaxMhz) ? std::optional<uint32_t>(ui32_MaxMhz) : std::nullopt;
}

// TODO: remove in-body returning
std::optional<uint32_t> sfopt_ReadGpuMaxFreqMhz(const std::string& krstr_PciDevDir) {
    // 1. Intel i915 / xe via drm/card*
    const std::string str_DrmDirPath = krstr_PciDevDir + std::string(ksv_SysSubdirDrm);
    if (UniqueDir pDrmDir(::opendir(str_DrmDirPath.c_str())); pDrmDir) {
        struct dirent* p_Entry = nullptr;
        while ((p_Entry = ::readdir(pDrmDir.get())) != nullptr) {
            const std::string_view sv_Name(p_Entry->d_name);
            if (!sv_Name.starts_with(ksv_CardPrefix) || std::string_view::npos != sv_Name.find(kc_CardConnSep)) {
                continue;
            }
            const std::string str_CardDir = str_DrmDirPath + "/" + std::string(sv_Name);
            uint64_t ui64_Mhz = 0ULL;
            for (std::string_view sv_FreqFile : ska_IntelFreqFiles) {
                if (FileReader::fb_ReadUint64(ui64_Mhz, str_CardDir + std::string(sv_FreqFile))) {
                    return static_cast<uint32_t>(ui64_Mhz);
                }
            }
        }
    }

    // 2. AMD amdgpu pp_dpm_sclk (MHz)
    if (std::optional<uint32_t> opt_Mhz =
            sfopt_ReadAmdDpmMaxMhz(krstr_PciDevDir + std::string(ksv_SysFileAmdDpmSclk))) {
        return opt_Mhz;
    }

    // 3. hwmon freq1_max (Hz -> MHz)
    const std::string str_HwmonRoot = krstr_PciDevDir + std::string(ksv_SysSubdirHwmon);
    if (UniqueDir pHwmonDir(::opendir(str_HwmonRoot.c_str())); pHwmonDir) {
        struct dirent* p_Entry = nullptr;
        while ((p_Entry = ::readdir(pHwmonDir.get())) != nullptr) {
            const std::string_view sv_Name(p_Entry->d_name);
            if (!sv_Name.starts_with(ksv_HwmonPrefix)) {
                continue;
            }
            uint64_t ui64_Hz = 0ULL;
            const std::string str_FreqPath =
                str_HwmonRoot + "/" + std::string(sv_Name) + std::string(ksv_SysFileHwmonMaxFreq);
            if (FileReader::fb_ReadUint64(ui64_Hz, str_FreqPath) && Units::kui64_HzPerMhz <= ui64_Hz) {
                return static_cast<uint32_t>(ui64_Hz / Units::kui64_HzPerMhz);
            }
        }
    }

    return std::nullopt;
}

} // namespace

/**
 * @brief Construct CGpuInfo and scan sysfs PCI devices for display controllers.
 */
CGpuInfo::CGpuInfo() {
    fv_DetectDevices();
}

/**
 * @brief Scan /sys/bus/pci/devices for display controllers and resolve names/clocks.
 */
void CGpuInfo::fv_DetectDevices() {
    UniqueDir pDir(::opendir(Paths::kpc_SysPciDevBus));
    if (!pDir) {
        return;
    }

    std::vector<std::string> vecstr_BusAddresses;
    struct dirent* p_Entry = nullptr;
    while (nullptr != (p_Entry = ::readdir(pDir.get()))) {
        const std::string_view sv_DevName(p_Entry->d_name);
        if (std::string_view::npos == sv_DevName.find(kc_PciAddrSep)) {
            continue;
        }
        const std::string str_ClassPath =
            std::string(Paths::kpc_SysPciDevBus) + "/" + std::string(sv_DevName) + std::string(ksv_SysFileClass);
        uint64_t ui64_PciClass = 0ULL;
        if (FileReader::fb_ReadUint64(ui64_PciClass, str_ClassPath, NumericBase::ki32_Hex) &&
            (Pci::kui32_ClassDisplayCtrl == (ui64_PciClass >> Pci::kui32_ClassCodeShift))) {
            vecstr_BusAddresses.emplace_back(sv_DevName);
        }
    }

    std::sort(vecstr_BusAddresses.begin(), vecstr_BusAddresses.end());

    for (const std::string& krstr_Addr : vecstr_BusAddresses) {
        const std::string str_DevDir = std::string(Paths::kpc_SysPciDevBus) + "/" + krstr_Addr;
        uint64_t ui64_Vendor = 0ULL;
        uint64_t ui64_Device = 0ULL;
        if (!FileReader::fb_ReadUint64(ui64_Vendor, str_DevDir + std::string(ksv_SysFileVendor), NumericBase::ki32_Hex) ||
            !FileReader::fb_ReadUint64(ui64_Device, str_DevDir + std::string(ksv_SysFileDevice), NumericBase::ki32_Hex)) {
            continue;
        }

        const uint16_t ui16_VendorId = static_cast<uint16_t>(ui64_Vendor);
        const uint16_t ui16_DeviceId = static_cast<uint16_t>(ui64_Device);

        std::string str_GpuName;
        std::string_view sv_VendorName;
        std::string_view sv_DeviceName;
        std::string str_SysVendor;
        std::string str_SysDevice;

        if (fb_LookupPciDevice(ui16_VendorId, ui16_DeviceId, sv_VendorName, sv_DeviceName)) {
            str_GpuName = sfstr_FormatGpuName(sv_VendorName, sv_DeviceName);
    #if 0
        } else if (sfb_LookupSystemPciIdsFile(ui16_VendorId, ui16_DeviceId, sv_VendorName, sv_DeviceName)) {
            str_GpuName = sfstr_FormatGpuName(sv_VendorName, sv_DeviceName);
    #endif
        } else {
            str_GpuName = StringHandler::fstr_Format(kpc_UnknownPciDisplayFmt,
                                                     ui16_VendorId, ui16_DeviceId);
        }

        mvec_Devices.push_back(gpu_device_t{
            krstr_Addr,
            std::move(str_GpuName),
            sfopt_ReadGpuMaxFreqMhz(str_DevDir)
        });
    }
}
