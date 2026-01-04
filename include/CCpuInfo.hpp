#ifndef _CCPUINFO_HPP_
#define _CCPUINFO_HPP_

#include <cstdint>
#include <string>

/**
 * @brief A class to get CPU information
 */
class CCpuInfo {
public:
    CCpuInfo(uint32_t ui32_CpuId = 0);
    CCpuInfo(const CCpuInfo&) = default;
    CCpuInfo& operator=(const CCpuInfo&) = default;
    CCpuInfo(CCpuInfo&&) = default;
    CCpuInfo& operator=(CCpuInfo&&) = default;
    virtual ~CCpuInfo() = default;

    /**
     * @brief Get the name of CPU
     *
     * Read filesystem and return CPU name
     *
     * @param None
     *
     * @return A std::string - The CPU name
     */
    std::string fstr_GetName() const;

    /**
     * @brief Get the id of CPU
     *
     * @param None
     *
     * @return An uint32_t number - The CPU ID
     */
    uint32_t fui32_GetCpuId() const;

    /**
     * @brief Get the maximum freqency of CPU
     *
     * Read filesystem and return the maximum freqency of CPU
     *
     * @param None
     *
     * @return An uint32_t number - The maximum CPU freqency or 0xFFFFFFFF on errors
     */
    uint32_t fui32_GetMaxFreq() const;

    /**
     * @brief Get the minimum freqency of CPU
     *
     * Read filesystem and return the minimum freqency of CPU
     *
     * @param None
     *
     * @return An uint32_t number - The minimum CPU freqency or 0xFFFFFFFF on errors
     */
    uint32_t fui32_GetMinFreq() const;

    /**
     * @brief Get the cache size of CPU
     *
     * Read filesystem and return CPU cache size
     *
     * @param None
     *
     * @return An uint32_t number - CPU cache size in kB or 0xFFFFFFFF on errors
     */
    uint32_t fui32_GetCache() const;

    /**
     * @brief Get the number of CPU logical cores
     *
     * Read filesystem and return the number of CPU logical cores
     *
     * @param None
     *
     * @return An uint32_t number - The number of CPU logical cores or 0xFFFFFFFF on errors
     */
    uint32_t fui32_GetLogicalCores() const;

    /**
     * @brief Get the number of CPU physical cores
     *
     * Read filesystem and return the number of CPU physical cores
     *
     * @param None
     *
     * @return An uint32_t number - The number of CPU physical cores or 0xFFFFFFFF on errors
     */
    uint32_t fui32_GetPhysicalCores() const;

private:
    /**
     * @brief Read cpuinfo filesystem and extract field values
     *
     * @param None
     *
     * @return None
     */
    void _fv_ReadCpuInfoFS() const;

    /**
     * @brief Read cpu_max_freq filesystem and get the maximum frequency
     *
     * @param None
     *
     * @return None
     */
    void _fv_ReadCpuMaxFreqFS(uint32_t ui32_Processor) const;

    /**
     * @brief Read cpu_mix_freq filesystem and get the minimum frequency
     *
     * @param None
     *
     * @return None
     */
    void _fv_ReadCpuMinFreqFS(uint32_t ui32_Processor) const;

    mutable std::string mstr_Name;
    mutable uint32_t mui32_CpuId;
    mutable uint32_t mui32_MaxFreq;
    mutable uint32_t mui32_MinFreq;
    mutable uint32_t mui32_Cache;
    mutable uint32_t mui32_LogicalCores;
    mutable uint32_t mui32_PhysicalCores;
};

#endif
