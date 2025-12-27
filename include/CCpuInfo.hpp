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
    std::string fstr_GetName();

    /**
     * @brief Get the id of CPU
     *
     * @param None
     *
     * @return An uint32_t number - The CPU ID
     */
    uint32_t fui32_GetCpuId();

    /**
     * @brief Get the maximum freqency of CPU
     *
     * Read filesystem and return the maximum freqency of CPU
     *
     * @param None
     *
     * @return An uint32_t number - The maximum CPU freqency or 0xFFFFFFFF on errors
     */
    uint32_t fui32_GetMaxFreq();

    /**
     * @brief Get the minimum freqency of CPU
     *
     * Read filesystem and return the minimum freqency of CPU
     *
     * @param None
     *
     * @return An uint32_t number - The minimum CPU freqency or 0xFFFFFFFF on errors
     */
    uint32_t fui32_GetMinFreq();

    /**
     * @brief Get the cache size of CPU
     *
     * Read filesystem and return CPU cache size
     *
     * @param None
     *
     * @return An uint32_t number - CPU cache size in kB or 0xFFFFFFFF on errors
     */
    uint32_t fui32_GetCache();

    /**
     * @brief Get the number of CPU logical cores
     *
     * Read filesystem and return the number of CPU logical cores
     *
     * @param None
     *
     * @return An uint32_t number - The number of CPU logical cores or 0xFFFFFFFF on errors
     */
    uint32_t fui32_GetLogicalCores();

    /**
     * @brief Get the number of CPU physical cores
     *
     * Read filesystem and return the number of CPU physical cores
     *
     * @param None
     *
     * @return An uint32_t number - The number of CPU physical cores or 0xFFFFFFFF on errors
     */
    uint32_t fui32_GetPhysicalCores();

private:
    /**
     * @brief Read cpuinfo filesystem and extract field values
     *
     * @param None
     *
     * @return None
     */
    void _fv_ReadCpuInfoFS();

    /**
     * @brief Read cpu_max_freq filesystem and get the maximum frequency
     *
     * @param None
     *
     * @return None
     */
    void _fv_ReadCpuMaxFreqFS();

    /**
     * @brief Read cpu_mix_freq filesystem and get the minimum frequency
     *
     * @param None
     *
     * @return None
     */
    void _fv_ReadCpuMinFreqFS();

    std::string mstr_Name;
    uint32_t mui32_CpuId;
    uint32_t mui32_MaxFreq;
    uint32_t mui32_MinFreq;
    uint32_t mui32_Cache;
    uint32_t mui32_LogicalCores;
    uint32_t mui32_PhysicalCores;

    // These variables will be gotten from cpuinfo file and used to get min/max freq
    uint32_t mui32_ReadPhysicalId;
    uint32_t mui32_ReadProcessor0;
};

#endif
