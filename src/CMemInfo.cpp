#include "CMemInfo.hpp"
#include "helpers/StringHandler.hpp"

#include <charconv>
#include <fstream>
#include <string>
#include <string_view>

namespace {

constexpr const char*      kpc_PathMemInfo    = "/proc/meminfo";
constexpr std::string_view ksv_KeyMemTotal    = "MemTotal:";
constexpr std::string_view ksv_KeyMemAvail    = "MemAvailable:";
constexpr std::string_view ksv_KeyMemFree     = "MemFree:";
constexpr std::string_view ksv_KeyShmem       = "Shmem:";
constexpr std::string_view ksv_KeyBuffers     = "Buffers:";
constexpr std::string_view ksv_KeyCached      = "Cached:";
constexpr std::string_view ksv_KeySwapTotal   = "SwapTotal:";
constexpr std::string_view ksv_KeySwapFree    = "SwapFree:";
constexpr std::string_view ksv_KeyKReclaim    = "KReclaimable:";
constexpr std::string_view ksv_KeySReclaim    = "SReclaimable:";
constexpr std::string_view ksv_KeyKernelStack = "KernelStack:";
constexpr std::string_view ksv_KeyPageTables  = "PageTables:";
constexpr std::string_view ksv_KeyVmallocTot  = "VmallocTotal:";
constexpr std::string_view ksv_KeyVmallocUsed = "VmallocUsed:";
constexpr std::string_view ksv_KeyVmallocChk  = "VmallocChunk:";

bool sfb_ParseMemLineValue(std::string_view sv_Line,
                           std::string_view sv_Key,
                           uint64_t& rui64_OutKb) {
    if (!sv_Line.starts_with(sv_Key)) {
        return false;
    }
    sv_Line.remove_prefix(sv_Key.size());
    StringHandler::fv_TrimStrView(sv_Line);
    uint64_t ui64_Val = 0ULL;
    const char* kpc_Begin = sv_Line.data();
    const char* kpc_End = sv_Line.data() + sv_Line.size();
    const auto [p_Ptr, ec] = std::from_chars(kpc_Begin, kpc_End, ui64_Val, NumericBase::ki32_Decimal);
    if (std::errc{} == ec && kpc_Begin != p_Ptr) {
        rui64_OutKb = ui64_Val;
        return true;
    }
    return false;
}

} // namespace

/**
 * @brief Construct CMemInfo and parse /proc/meminfo.
 */
CMemInfo::CMemInfo() {
    fv_ReadMemInfoFs();
}

/**
 * @brief Get used memory in KiB (Total - Available, or Total - Free fallback).
 *
 * @return Used memory in KiB
 */
uint64_t CMemInfo::fui64_GetUsed() const noexcept {
    if (mb_HasAvailable) {
        return mui64_Total >= mui64_Available ? (mui64_Total - mui64_Available) : 0ULL;
    }
    return mui64_Total >= mui64_Free ? (mui64_Total - mui64_Free) : 0ULL;
}

/**
 * @brief Read and parse memory metrics from /proc/meminfo in a single pass.
 */
void CMemInfo::fv_ReadMemInfoFs() {
    std::ifstream ifs(kpc_PathMemInfo);
    if (!ifs.is_open()) {
        return;
    }

    typedef struct {
        std::string_view sv_Key;
        uint64_t* pui64_Target;
        bool* pb_FoundFlag;
    } mem_mapping_t;

    mem_mapping_t a_Mappings[] = {
        { ksv_KeyMemTotal,    &mui64_Total,        nullptr },
        { ksv_KeyMemAvail,    &mui64_Available,    &mb_HasAvailable },
        { ksv_KeyMemFree,     &mui64_Free,         nullptr },
        { ksv_KeyShmem,       &mui64_Shmem,        nullptr },
        { ksv_KeyBuffers,     &mui64_Buffers,      nullptr },
        { ksv_KeyCached,      &mui64_Cached,       nullptr },
        { ksv_KeySwapTotal,   &mui64_TotalSwap,    nullptr },
        { ksv_KeySwapFree,    &mui64_FreeSwap,     nullptr },
        { ksv_KeyKReclaim,    &mui64_KReclaimable, nullptr },
        { ksv_KeySReclaim,    &mui64_SReclaimable, nullptr },
        { ksv_KeyKernelStack, &mui64_KernelStack,  nullptr },
        { ksv_KeyPageTables,  &mui64_PageTables,   nullptr },
        { ksv_KeyVmallocTot,  &mui64_VmallocTotal, nullptr },
        { ksv_KeyVmallocUsed, &mui64_VmallocUsed,  nullptr },
        { ksv_KeyVmallocChk,  &mui64_VmallocChunk, nullptr },
    };

    std::string str_Line;
    while (std::getline(ifs, str_Line)) {
        for (mem_mapping_t& r_Map : a_Mappings) {
            if (sfb_ParseMemLineValue(str_Line, r_Map.sv_Key, *r_Map.pui64_Target)) {
                if (nullptr != r_Map.pb_FoundFlag) {
                    *r_Map.pb_FoundFlag = true;
                }
                break;
            }
        }
    }
}
