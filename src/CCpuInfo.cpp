#include "CCpuInfo.hpp"
#include "utils/Definitions.hpp"
#include "helpers/FileReader.hpp"
#include "helpers/RaiiHandles.hpp"
#include "helpers/StringHandler.hpp"

#include <dirent.h>
#include <unistd.h>
#include <array>
#include <charconv>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include <set>
#include <thread>
#include <utility>

namespace {

constexpr const char* kpc_PathCpuInfo         = "/proc/cpuinfo";
constexpr const char* kpc_SysCpuDir           = "/sys/devices/system/cpu";
constexpr const char* kpc_SysCpufreqDir       = "/sys/devices/system/cpu/cpufreq";
constexpr const char* kpc_SysCpu0Cpufreq      = "/sys/devices/system/cpu/cpu0/cpufreq";

constexpr std::string_view ksv_KeyModelName   = "model name";
constexpr std::string_view ksv_KeyModel       = "Model";
constexpr std::string_view ksv_KeyHardware    = "Hardware";
constexpr std::string_view ksv_KeyProcessor   = "Processor";
constexpr std::string_view ksv_KeyPhysicalId  = "physical id";
constexpr std::string_view ksv_KeySiblings    = "siblings";
constexpr std::string_view ksv_KeyCpuCores    = "cpu cores";
constexpr std::string_view ksv_KeyCacheSize   = "cache size";
constexpr std::string_view ksv_KeyCpuMhz      = "cpu MHz";

constexpr std::string_view ksv_CpuPrefix      = "cpu";
constexpr std::string_view ksv_PolicyPrefix   = "policy";
constexpr std::string_view ksv_TopoSubdir     = "/topology";
constexpr std::string_view ksv_CoreIdFile     = "/core_id";
constexpr std::string_view ksv_PkgIdFile      = "/physical_package_id";
constexpr std::string_view ksv_CpuMaxFreqFile = "/cpuinfo_max_freq";
constexpr std::string_view ksv_ScalingMaxFile = "/scaling_max_freq";
constexpr std::string_view ksv_CpuMinFreqFile = "/cpuinfo_min_freq";
constexpr std::string_view ksv_ScalingMinFile = "/scaling_min_freq";

constexpr char kc_FreqSepChar = '@';
constexpr char kc_KeyValSep   = ':';
constexpr char kc_SpaceChar   = ' ';

constexpr std::array<std::string_view, 4> ska_StripCpuTokens = {
    "(R)", "(TM)", "(tm)", " CPU"
};

// TODO: Improve performace of `sfstr_CleanCpuName` function
std::string sfstr_CleanCpuName(std::string krstr_Name) {
    // Strip "@ X.XXGHz" suffix if present
    if (const std::size_t ui64_AtPos = krstr_Name.find(kc_FreqSepChar); std::string::npos != ui64_AtPos) {
        krstr_Name.erase(ui64_AtPos);
    }
    for (std::string_view sv_Token : ska_StripCpuTokens) {
        krstr_Name = StringHandler::fstr_ReplaceAll(krstr_Name, sv_Token, "");
    }

    // Collapse consecutive spaces
    std::string str_Collapsed;
    str_Collapsed.reserve(krstr_Name.size());
    bool b_PrevSpace = false;
    for (unsigned char uc_Ch : krstr_Name) {
        if (std::isspace(uc_Ch)) {
            if (!b_PrevSpace && !str_Collapsed.empty()) {
                str_Collapsed.push_back(kc_SpaceChar);
                b_PrevSpace = true;
            }
        } else {
            str_Collapsed.push_back(static_cast<char>(uc_Ch));
            b_PrevSpace = false;
        }
    }
    StringHandler::fv_Trim(str_Collapsed);
    return str_Collapsed;
}

std::string_view sfsv_ExtractCpuInfoValue(const std::string_view& krsv_Line) {
    const std::size_t ui64_Colon = krsv_Line.find(kc_KeyValSep);
    if (std::string_view::npos == ui64_Colon) {
        return {};
    }
    std::string_view sv_Val = krsv_Line.substr(ui64_Colon + 1U);
    // TODO: support left and right trim for string_view type
    while (!sv_Val.empty() && (sv_Val.front() == ' ' || sv_Val.front() == '\t')) {
        sv_Val.remove_prefix(1U);
    }
    while (!sv_Val.empty() && (sv_Val.back() == ' ' || sv_Val.back() == '\t' || sv_Val.back() == '\r')) {
        sv_Val.remove_suffix(1U);
    }
    return sv_Val;
}

bool sfb_ParseUint32(const std::string_view& krsv_Val, uint32_t& rui32_Out) {
    const char* kpc_Begin = krsv_Val.data();
    const char* kpc_End = krsv_Val.data() + krsv_Val.size();
    const auto [p_Ptr, errCode] = std::from_chars(kpc_Begin, kpc_End, rui32_Out, NumericBase::ki32_Decimal);
    return errCode == std::errc{} && p_Ptr != kpc_Begin;
}

bool sfb_IsCpuSubdirName(const std::string_view& krsv_DirName) {
    if (!krsv_DirName.starts_with(ksv_CpuPrefix) || ksv_CpuPrefix.size() >= krsv_DirName.size()) {
        return false;
    }
    for (unsigned char uc_Ch : krsv_DirName.substr(ksv_CpuPrefix.size())) {
        if (!std::isdigit(uc_Ch)) {
            return false;
        }
    }
    return true;
}

} // namespace

/**
 * @brief Construct CCpuInfo and gather CPU details for the target physical CPU ID.
 *
 * @param ui32_CpuId - Physical CPU package ID (defaults to 0)
 */
CCpuInfo::CCpuInfo(uint32_t ui32_CpuId) : mui32_CpuId(ui32_CpuId) {
    fv_ReadCpuInfoFs();
    // Retrive other infomation only if found a cpu matched input CPU ID
    if (0U != mui32_PhysicalCores && 0U != mui32_LogicalCores) {
#if 0
    fv_ReadCoreTopology();
#endif
        fv_ReadFrequencies();
    }
}

/**
 * @brief Parse /proc/cpuinfo for model name, cache size, and fallback frequencies.
 */
void CCpuInfo::fv_ReadCpuInfoFs() {
    std::ifstream ifs(kpc_PathCpuInfo);
    if (!ifs.is_open()) {
        return;
    }

    std::string str_BlockModel;
    uint32_t ui32_BlockPhysicalId = 0U;
    uint32_t ui32_BlockSiblings = 0U;
    uint32_t ui32_BlockCores = 0U;
    uint32_t ui32_BlockCache = 0U;
    bool b_MatchedBlock = false;

    auto fn_CommitBlockIfMatches = [&]() {
        if (b_MatchedBlock) {
            return;
        }
        if (mui32_CpuId == ui32_BlockPhysicalId) {
            if (!str_BlockModel.empty()) {
                mstr_Name = sfstr_CleanCpuName(str_BlockModel);
            }
            if (0U < ui32_BlockSiblings) {
                mui32_LogicalCores = ui32_BlockSiblings;
            }
            if (0U < ui32_BlockCores) {
                mui32_PhysicalCores = ui32_BlockCores;
            }
            if (0U < ui32_BlockCache) {
                moptui32_CacheKb = ui32_BlockCache;
            }
            b_MatchedBlock = true;
        }
        str_BlockModel.clear();
        ui32_BlockPhysicalId = 0U;
        ui32_BlockSiblings = 0U;
        ui32_BlockCores = 0U;
        ui32_BlockCache = 0U;
    };

    std::string str_Line;
    while (std::getline(ifs, str_Line)) {
        if (str_Line.empty()) {
            fn_CommitBlockIfMatches();
            continue;
        }

        const std::string_view sv_Line(str_Line);
        if (sv_Line.starts_with(ksv_KeyModelName)) {
            str_BlockModel = std::string(sfsv_ExtractCpuInfoValue(sv_Line));

        } else if (sv_Line.starts_with(ksv_KeyPhysicalId)) {
            sfb_ParseUint32(sfsv_ExtractCpuInfoValue(sv_Line), ui32_BlockPhysicalId);

        } else if (sv_Line.starts_with(ksv_KeySiblings)) {
            sfb_ParseUint32(sfsv_ExtractCpuInfoValue(sv_Line), ui32_BlockSiblings);

        } else if (sv_Line.starts_with(ksv_KeyCpuCores)) {
            sfb_ParseUint32(sfsv_ExtractCpuInfoValue(sv_Line), ui32_BlockCores);

        } else if (sv_Line.starts_with(ksv_KeyCacheSize)) {
            sfb_ParseUint32(sfsv_ExtractCpuInfoValue(sv_Line), ui32_BlockCache);
        }
    }

    // Handle files that do not end with a blank line
    fn_CommitBlockIfMatches();
}

#if 0
/**
 * @brief Query sysconf and sysfs topology for logical and physical core counts.
 */
void CCpuInfo::fv_ReadCoreTopology() {
    const long i64_Online = ::sysconf(_SC_NPROCESSORS_ONLN);
    if (0L < i64_Online) {
        mui32_LogicalCores = static_cast<uint32_t>(i64_Online);
    } else if (0U == mui32_LogicalCores) {
        mui32_LogicalCores = std::thread::hardware_concurrency();
    }

    UniqueDir pDir(::opendir(kpc_SysCpuDir));
    if (pDir) {
        std::set<std::pair<uint64_t, uint64_t>> set_UniqueCores;
        struct dirent* p_Entry = nullptr;
        while (nullptr != (p_Entry = ::readdir(pDir.get()))) {
            if (!sfb_IsCpuSubdirName(p_Entry->d_name)) {
                continue;
            }
            const std::string str_TopoDir =
                std::string(kpc_SysCpuDir) + "/" + p_Entry->d_name + std::string(ksv_TopoSubdir);
            uint64_t ui64_PkgId = 0ULL;
            uint64_t ui64_CoreId = 0ULL;
            if (FileReader::fb_ReadUint64(ui64_CoreId, str_TopoDir + std::string(ksv_CoreIdFile))) {
                FileReader::fb_ReadUint64(ui64_PkgId, str_TopoDir + std::string(ksv_PkgIdFile));
                set_UniqueCores.emplace(ui64_PkgId, ui64_CoreId);
            }
        }
        if (!set_UniqueCores.empty()) {
            mui32_PhysicalCores = static_cast<uint32_t>(set_UniqueCores.size());
        }
    }

    if (0U == mui32_PhysicalCores) {
        mui32_PhysicalCores = mui32_LogicalCores;
    }
}
#endif

/**
 * @brief Scan sysfs cpufreq policies for maximum and minimum frequencies.
 */
void CCpuInfo::fv_ReadFrequencies() {
    std::optional<uint64_t> optui64_MaxSysFreq;
    std::optional<uint64_t> optui64_MinSysFreq;

    auto fn_UpdateRange = [&](const std::string& krstr_FreqDir) {
        uint64_t ui64_Val = 0ULL;
        if (FileReader::fb_ReadUint64(ui64_Val, krstr_FreqDir + std::string(ksv_CpuMaxFreqFile)) ||
            FileReader::fb_ReadUint64(ui64_Val, krstr_FreqDir + std::string(ksv_ScalingMaxFile))) {
            if (!optui64_MaxSysFreq || ui64_Val > *optui64_MaxSysFreq) {
                optui64_MaxSysFreq = ui64_Val;
            }
        }

        if (FileReader::fb_ReadUint64(ui64_Val, krstr_FreqDir + std::string(ksv_CpuMinFreqFile)) ||
            FileReader::fb_ReadUint64(ui64_Val, krstr_FreqDir + std::string(ksv_ScalingMinFile))) {
            if (!optui64_MinSysFreq || ui64_Val < *optui64_MinSysFreq) {
                optui64_MinSysFreq = ui64_Val;
            }
        }
    };

    // Scan all cpufreq policies (handles Intel P/E-cores and ARM big.LITTLE)
    if (UniqueDir pPolicyDir(::opendir(kpc_SysCpufreqDir)); pPolicyDir) {
        struct dirent* p_Entry = nullptr;
        while ((p_Entry = ::readdir(pPolicyDir.get())) != nullptr) {
            if (std::string_view(p_Entry->d_name).starts_with(ksv_PolicyPrefix)) {
                fn_UpdateRange(std::string(kpc_SysCpufreqDir) + "/" + p_Entry->d_name);
            }
        }
    }

    // Fallback to cpu0/cpufreq if policy* directories are not present
    if (!optui64_MaxSysFreq) {
        fn_UpdateRange(kpc_SysCpu0Cpufreq);
    }

    if (optui64_MaxSysFreq) {
        moptui64_MaxFreqKhz = *optui64_MaxSysFreq;
    }
    if (optui64_MinSysFreq) {
        moptui64_MinFreqKhz = *optui64_MinSysFreq;
    }
}

