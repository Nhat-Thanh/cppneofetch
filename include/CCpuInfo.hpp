#ifndef CPPNEOFETCH_CCPUINFO_HPP
#define CPPNEOFETCH_CCPUINFO_HPP

#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief A class to get CPU model, core counts, cache size, and frequency.
 */
class CCpuInfo {
public:
    /**
     * @brief Construct CCpuInfo and gather CPU details for the target physical CPU ID.
     *
     * @param ui32_CpuId - Physical CPU package ID (defaults to 0)
     */
    explicit CCpuInfo(uint32_t ui32_CpuId = 0U);

    /**
     * @brief Get the cleaned CPU model name.
     *
     * @return Const reference to the CPU model name string
     */
    const std::string& fstr_GetName() const noexcept { return mstr_Name; }

    /**
     * @brief Get the target physical CPU ID.
     *
     * @return Physical CPU ID
     */
    uint32_t fui32_GetCpuId() const noexcept { return mui32_CpuId; }

    /**
     * @brief Get the maximum CPU frequency in kHz, if available.
     *
     * @return Optional maximum frequency in kHz
     */
    std::optional<uint64_t> foptui64_GetMaxFreqKhz() const noexcept { return moptui64_MaxFreqKhz; }

    /**
     * @brief Get the minimum CPU frequency in kHz, if available.
     *
     * @return Optional minimum frequency in kHz
     */
    std::optional<uint64_t> foptui64_GetMinFreqKhz() const noexcept { return moptui64_MinFreqKhz; }

    /**
     * @brief Get the CPU cache size in KiB, if available.
     *
     * @return Optional cache size in KiB
     */
    std::optional<uint32_t> foptui32_GetCacheKb() const noexcept { return moptui32_CacheKb; }

    /**
     * @brief Get the number of online logical CPU threads.
     *
     * @return Logical core/thread count
     */
    uint32_t fui32_GetLogicalCores() const noexcept { return mui32_LogicalCores; }


    /**
     * @brief Get the number of unique physical CPU cores.
     *
     * @return Physical core count
     */
    uint32_t fui32_GetPhysicalCores() const noexcept { return mui32_PhysicalCores; }

private:
    // TODO: Support multi-socket system
    /**
     * @brief Parse /proc/cpuinfo for model name, cache size, and fallback frequencies.
     */
    void fv_ReadCpuInfoFs();

#if 0
    // TODO: An sysfs approach to get physical and logical cores number, will be useful in future
    /**
     * @brief Query sysconf and sysfs topology for logical and physical core counts.
     */
    void fv_ReadCoreTopology();
#endif

    // TODO: Support multi-socket system
    /**
     * @brief Scan sysfs cpufreq policies for maximum and minimum frequencies.
     */
    void fv_ReadFrequencies();

    // TODO: Support multi-socket system
    uint32_t mui32_CpuId;
    std::string mstr_Name;
    std::optional<uint64_t> moptui64_MaxFreqKhz{0UL};
    std::optional<uint64_t> moptui64_MinFreqKhz{0UL};
    std::optional<uint32_t> moptui32_CacheKb{0UL};
    uint32_t mui32_LogicalCores{0U};
    uint32_t mui32_PhysicalCores{0U};
};

#endif // CPPNEOFETCH_CCPUINFO_HPP
